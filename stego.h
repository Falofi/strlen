/* Partie commune à ft_strlen_stego.c et recover.c : lecture du fond d'écran.

 * Compiler les programmes avec -ljpeg -lpng.
 * Variables d'environnement (utiles pour tester sans toucher au bureau) :
 *   STRLEN_WALLPAPER  chemin de l'image d'entrée (sinon : gsettings)
 */

#ifndef STEGO_H
# define STEGO_H

# include <ctype.h>
# include <limits.h>
# include <setjmp.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <jpeglib.h>
# include <png.h>

/* Image en RGB 8 bits, octets dans l'ordre R,V,B, ligne par ligne. */
typedef struct s_image
{
	int				w;
	int				h;
	unsigned char	*rgb;
}	t_image;

static int	get_wallpaper_uri(char *uri, size_t size)
{
	FILE	*p;
	size_t	n;

	p = popen("gsettings get org.gnome.desktop.background picture-uri", "r");
	if (!p)
		return (0);
	if (!fgets(uri, (int)size, p))
	{
		pclose(p);
		return (0);
	}
	pclose(p);
	n = strlen(uri);
	while (n > 0 && (uri[n - 1] == '\n' || uri[n - 1] == '\''))
		uri[--n] = '\0';
	if (uri[0] == '\'')
		memmove(uri, uri + 1, strlen(uri));
	return (1);
}

/* Transforme file:///a%20b.jpg en /a b.jpg dans path. */
static int	uri_to_path(const char *uri, char *path, size_t size)
{
	const char		*src;
	size_t			j;
	unsigned int	v;

	if (strncmp(uri, "file://", 7) != 0)
		return (0);
	src = uri + 7;
	j = 0;
	while (*src && j + 1 < size)
	{
		if (*src == '%' && isxdigit((unsigned char)src[1])
			&& isxdigit((unsigned char)src[2]))
		{
			sscanf(src + 1, "%2x", &v);
			path[j++] = (char)v;
			src += 3;
		}
		else
			path[j++] = *src++;
	}
	path[j] = '\0';
	return (1);
}

/* Chemin du fond d'écran actuel (STRLEN_WALLPAPER en test, sinon gsettings). */
static inline int	get_wallpaper_path(char *path, size_t size)
{
	char	uri[1024];

	if (getenv("STRLEN_WALLPAPER"))
	{
		snprintf(path, size, "%s", getenv("STRLEN_WALLPAPER"));
		return (1);
	}
	if (!get_wallpaper_uri(uri, sizeof(uri)))
		return (0);
	return (uri_to_path(uri, path, size));
}

static int	load_png(FILE *fp, t_image *img)
{
	png_structp	png;
	png_infop	info;
	png_bytep	*rows;
	int			y;

	png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info = png_create_info_struct(png);
	if (!png || !info || setjmp(png_jmpbuf(png)))
	{
		png_destroy_read_struct(&png, &info, NULL);
		return (0);
	}
	png_init_io(png, fp);
	png_read_info(png, info);
	/* Normalisation : toujours RGB 8 bits, sans alpha, sans palette. */
	png_set_expand(png);
	png_set_strip_16(png);
	png_set_gray_to_rgb(png);
	png_set_strip_alpha(png);
	png_read_update_info(png, info);
	img->w = png_get_image_width(png, info);
	img->h = png_get_image_height(png, info);
	if (png_get_rowbytes(png, info) != (size_t)img->w * 3)
	{
		png_destroy_read_struct(&png, &info, NULL);
		return (0);
	}
	img->rgb = malloc((size_t)img->w * img->h * 3);
	rows = malloc(sizeof(*rows) * img->h);
	if (!img->rgb || !rows)
	{
		png_destroy_read_struct(&png, &info, NULL);
		return (0);
	}
	for (y = 0; y < img->h; y++)
		rows[y] = img->rgb + (size_t)y * img->w * 3;
	png_read_image(png, rows);
	png_destroy_read_struct(&png, &info, NULL);
	free(rows);
	return (1);
}

static int	load_jpeg(FILE *fp, t_image *img)
{
	struct jpeg_decompress_struct	cinfo;
	struct jpeg_error_mgr			jerr;
	JSAMPROW						row;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, fp);
	jpeg_read_header(&cinfo, TRUE);
	cinfo.out_color_space = JCS_RGB;
	jpeg_start_decompress(&cinfo);
	img->w = cinfo.output_width;
	img->h = cinfo.output_height;
	img->rgb = malloc((size_t)img->w * img->h * 3);
	if (!img->rgb)
	{
		jpeg_destroy_decompress(&cinfo);
		return (0);
	}
	while (cinfo.output_scanline < cinfo.output_height)
	{
		row = img->rgb + (size_t)cinfo.output_scanline * img->w * 3;
		jpeg_read_scanlines(&cinfo, &row, 1);
	}
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	return (1);
}

/* Charge un JPEG ou un PNG (détection par signature, pas par extension). */
static int	load_image(const char *path, t_image *img)
{
	FILE			*fp;
	unsigned char	sig[8];
	size_t			n;
	int				ok;

	fp = fopen(path, "rb");
	if (!fp)
		return (0);
	n = fread(sig, 1, sizeof(sig), fp);
	rewind(fp);
	ok = 0;
	if (n == 8 && memcmp(sig, "\x89PNG\r\n\x1a\n", 8) == 0)
		ok = load_png(fp, img);
	else if (n >= 3 && sig[0] == 0xFF && sig[1] == 0xD8 && sig[2] == 0xFF)
		ok = load_jpeg(fp, img);
	fclose(fp);
	return (ok);
}

#endif
