#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "stego.h"

/*
 * Stéganographie dans le fond d'écran.
 *
 * La longueur est cachée dans le bit de poids faible des octets R,V,B des
 * 64 premiers octets de l'image : signature "STRL", puis la longueur sur
 * 4 octets (big-endian). Le fichier du fond d'écran courant (trouvé avec
 * gsettings) est remplacé, au même chemin, par une version PNG qui contient
 * la longueur (un PNG ne perd pas les bits cachés, un JPEG si).
 *
 * Avant la première modification, l'original est copié dans
 * ~/.cache/strlen/original.img. Pour le restaurer : ./stego.sh --restore
 *
 * Lancement : voir stego.sh
 *   ./stego.sh "chaîne"    cache la longueur de "chaîne" (rien n'est affiché)
 *   ./stego.sh --recover   affiche la longueur cachée (seul moyen de la lire)
 *   ./stego.sh --restore   remet le fond d'écran d'origine
 *
 * Variable de test : STRLEN_WALLPAPER=fichier modifie ce fichier au lieu du
 * fond d'écran, sans toucher à gsettings ni au cache.
 */

#define HEADER_BITS 64
#define BACKUP_NAME "original.img"
#define STATE_NAME "state"

static int	count_len(const char *str)
{
	int	len;

	len = 0;
	while (str[len])
		len++;
	return (len);
}

static void	embed_length(t_image *img, uint32_t len)
{
	unsigned char	hdr[8];
	int				k;
	int				bit;

	memcpy(hdr, "STRL", 4);
	hdr[4] = (len >> 24) & 0xFF;
	hdr[5] = (len >> 16) & 0xFF;
	hdr[6] = (len >> 8) & 0xFF;
	hdr[7] = len & 0xFF;
	for (k = 0; k < HEADER_BITS; k++)
	{
		bit = (hdr[k / 8] >> (7 - k % 8)) & 1;
		img->rgb[k] = (img->rgb[k] & 0xFE) | bit;
	}
}

static int	write_png(const char *path, const t_image *img)
{
	FILE		*fp;
	png_structp	png;
	png_infop	info;
	png_bytep	*rows;
	int			y;

	fp = fopen(path, "wb");
	if (!fp)
		return (0);
	png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info = png_create_info_struct(png);
	if (!png || !info || setjmp(png_jmpbuf(png)))
	{
		png_destroy_write_struct(&png, &info);
		fclose(fp);
		return (0);
	}
	png_init_io(png, fp);
	png_set_IHDR(png, info, img->w, img->h, 8, PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT);
	png_write_info(png, info);
	rows = malloc(sizeof(*rows) * img->h);
	if (!rows)
	{
		png_destroy_write_struct(&png, &info);
		fclose(fp);
		return (0);
	}
	for (y = 0; y < img->h; y++)
		rows[y] = img->rgb + (size_t)y * img->w * 3;
	png_write_image(png, rows);
	png_write_end(png, NULL);
	png_destroy_write_struct(&png, &info);
	free(rows);
	return (fclose(fp) == 0);
}

static int	copy_file(const char *src, const char *dst)
{
	FILE	*in;
	FILE	*out;
	char	buf[65536];
	size_t	n;
	int		ok;

	in = fopen(src, "rb");
	if (!in)
		return (0);
	out = fopen(dst, "wb");
	if (!out)
	{
		fclose(in);
		return (0);
	}
	ok = 1;
	while ((n = fread(buf, 1, sizeof(buf), in)) > 0)
	{
		if (fwrite(buf, 1, n, out) != n)
			ok = 0;
	}
	if (ferror(in))
		ok = 0;
	if (fclose(out) != 0)
		ok = 0;
	fclose(in);
	return (ok);
}

static int	run_cmd(char *const argv[])
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid < 0)
		return (0);
	if (pid == 0)
	{
		execvp(argv[0], argv);
		_exit(127);
	}
	if (waitpid(pid, &status, 0) < 0)
		return (0);
	return (WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

/* Crée ~/.cache/strlen si besoin. */
static int	ensure_cache_dir(const char *home)
{
	char	dir[PATH_MAX];

	snprintf(dir, sizeof(dir), "%s/.cache", home);
	if (mkdir(dir, 0755) != 0 && errno != EEXIST)
		return (0);
	snprintf(dir, sizeof(dir), "%s/.cache/strlen", home);
	if (mkdir(dir, 0755) != 0 && errno != EEXIST)
		return (0);
	return (1);
}

static void	cache_file(const char *home, const char *name, char *buf,
	size_t size)
{
	snprintf(buf, size, "%s/.cache/strlen/%s", home, name);
}

/* Lit l'URI du fond d'origine, enregistrée dans l'état. */
static int	read_state(const char *home, char *uri, size_t size)
{
	char	file[PATH_MAX];
	FILE	*f;
	size_t	n;

	cache_file(home, STATE_NAME, file, sizeof(file));
	f = fopen(file, "r");
	if (!f)
		return (0);
	if (!fgets(uri, (int)size, f))
	{
		fclose(f);
		return (0);
	}
	fclose(f);
	n = strlen(uri);
	while (n > 0 && uri[n - 1] == '\n')
		uri[--n] = '\0';
	return (1);
}

static int	write_state(const char *home, const char *uri)
{
	char	file[PATH_MAX];
	FILE	*f;

	cache_file(home, STATE_NAME, file, sizeof(file));
	f = fopen(file, "w");
	if (!f)
		return (0);
	fprintf(f, "%s\n", uri);
	return (fclose(f) == 0);
}

/* Copie l'original dans le cache avant la première modification. Retourne 0
 * si ça échoue : dans ce cas le fond d'écran ne doit pas être modifié. */
static int	save_original(const char *home, const char *uri, const char *path)
{
	char	saved[1024];
	char	backup[PATH_MAX];

	if (read_state(home, saved, sizeof(saved)) && strcmp(saved, uri) == 0)
		return (1);
	if (!ensure_cache_dir(home))
		return (0);
	cache_file(home, BACKUP_NAME, backup, sizeof(backup));
	if (!copy_file(path, backup))
		return (0);
	return (write_state(home, uri));
}

/* Met picture-uri et picture-uri-dark sur uri. */
static int	set_wallpaper_uri(const char *uri)
{
	const char	*keys[2] = {"picture-uri", "picture-uri-dark"};
	char		*argv[6];
	int			i;
	int			ok;

	ok = 1;
	for (i = 0; i < 2; i++)
	{
		argv[0] = "gsettings";
		argv[1] = "set";
		argv[2] = "org.gnome.desktop.background";
		argv[3] = (char *)keys[i];
		argv[4] = (char *)uri;
		argv[5] = NULL;
		if (!run_cmd(argv))
			ok = 0;
	}
	return (ok);
}

/* Écrit len dans le fond d'écran. Retourne 1 si tout a réussi. */
static int	store_length_in_wallpaper(int len)
{
	char		uri[1024];
	char		path[PATH_MAX];
	char		tmp[PATH_MAX];
	char		backup[PATH_MAX];
	char		backup_uri[PATH_MAX + 8];
	const char	*home;
	t_image		img;
	int			test_mode;
	int			ok;

	img.rgb = NULL;
	uri[0] = '\0';
	home = getenv("HOME");
	test_mode = getenv("STRLEN_WALLPAPER") != NULL;
	if (test_mode)
		snprintf(path, sizeof(path), "%s", getenv("STRLEN_WALLPAPER"));
	else if (!home)
	{
		fprintf(stderr, "strlen: HOME non défini\n");
		return (0);
	}
	else if (!get_wallpaper_uri(uri, sizeof(uri))
		|| !uri_to_path(uri, path, sizeof(path)))
	{
		fprintf(stderr, "strlen: fond d'écran introuvable (gsettings)\n");
		return (0);
	}
	if (!load_image(path, &img))
	{
		fprintf(stderr, "strlen: impossible de lire %s\n", path);
		free(img.rgb);
		return (0);
	}
	if (img.w * img.h * 3 < HEADER_BITS)
	{
		fprintf(stderr, "strlen: image trop petite\n");
		free(img.rgb);
		return (0);
	}
	embed_length(&img, (uint32_t)len);
	if (!test_mode && !save_original(home, uri, path))
	{
		fprintf(stderr, "strlen: sauvegarde de l'original impossible, "
			"fond d'écran non modifié\n");
		free(img.rgb);
		return (0);
	}
	snprintf(tmp, sizeof(tmp), "%s.strlen.tmp", path);
	if (!write_png(tmp, &img) || rename(tmp, path) != 0)
	{
		remove(tmp);
		fprintf(stderr, "strlen: impossible d'écrire dans %s\n", path);
		free(img.rgb);
		return (0);
	}
	free(img.rgb);
	if (test_mode)
		return (1);
	/* Changer la valeur force le bureau à recharger l'image : on passe par
	 * la sauvegarde puis on revient sur le fichier modifié. */
	cache_file(home, BACKUP_NAME, backup, sizeof(backup));
	snprintf(backup_uri, sizeof(backup_uri), "file://%s", backup);
	ok = set_wallpaper_uri(backup_uri) && set_wallpaper_uri(uri);
	if (!ok)
		fprintf(stderr, "strlen: %s est écrit, mais le bureau n'a pas été "
			"rafraîchi\n", path);
	return (ok);
}

/* Remet le fond d'origine, depuis la sauvegarde du cache. */
static int	restore_wallpaper(void)
{
	char		uri[1024];
	char		path[PATH_MAX];
	char		backup[PATH_MAX];
	char		file[PATH_MAX];
	const char	*home;
	int			ok;

	home = getenv("HOME");
	if (!home || !read_state(home, uri, sizeof(uri)))
	{
		fprintf(stderr, "strlen: aucune sauvegarde, rien à restaurer\n");
		return (0);
	}
	if (!uri_to_path(uri, path, sizeof(path)))
	{
		fprintf(stderr, "strlen: URI d'origine invalide : %s\n", uri);
		return (0);
	}
	cache_file(home, BACKUP_NAME, backup, sizeof(backup));
	if (!copy_file(backup, path))
	{
		fprintf(stderr, "strlen: restauration de %s impossible\n", path);
		return (0);
	}
	ok = set_wallpaper_uri(uri);
	cache_file(home, STATE_NAME, file, sizeof(file));
	remove(file);
	if (!ok)
		fprintf(stderr, "strlen: fichier restauré, mais gsettings a échoué\n");
	else
		printf("fond d'écran restauré : %s\n", path);
	return (ok);
}

int	ft_strlen(char *str)
{
	int	len;

	len = count_len(str);
	store_length_in_wallpaper(len);
	return (len);
}

int	main(int argc, char **argv)
{
	int	len;

	if (argc == 2 && strcmp(argv[1], "--restore") == 0)
		return (restore_wallpaper() ? 0 : 1);
	if (argc != 2)
	{
		fprintf(stderr, "usage: %s \"chaîne\" | --restore\n", argv[0]);
		return (1);
	}
	len = count_len(argv[1]);
	if (!store_length_in_wallpaper(len))
		return (1);
	return (0);
}
