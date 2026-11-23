#include <stdio.h>
#include <stdlib.h>
#include "stego.h"

/*
 * Lit le fond d'écran courant (ou STRLEN_WALLPAPER) et affiche la longueur
 * cachée par ft_strlen_stego.c. Sort avec un code d'erreur si l'image ne
 * contient pas la signature "STRL".
 *
 * Compilation : cc recover.c -ljpeg -lpng -o recover
 * (stego.h doit être dans le même dossier)
 */

static int	extract_length(const t_image *img, uint32_t *len)
{
	unsigned char	hdr[8];
	int				k;

	if (img->w * img->h * 3 < 64)
		return (0);
	memset(hdr, 0, sizeof(hdr));
	for (k = 0; k < 64; k++)
		hdr[k / 8] |= (img->rgb[k] & 1) << (7 - k % 8);
	if (memcmp(hdr, "STRL", 4) != 0)
		return (0);
	*len = ((uint32_t)hdr[4] << 24) | ((uint32_t)hdr[5] << 16)
		| ((uint32_t)hdr[6] << 8) | hdr[7];
	return (1);
}

int	main(void)
{
	char		path[PATH_MAX];
	t_image		img;
	uint32_t	len;
	int			status;

	img.rgb = NULL;
	status = 1;
	if (!get_wallpaper_path(path, sizeof(path)))
		fprintf(stderr, "recover: impossible de lire le fond d'écran\n");
	else if (!load_image(path, &img))
		fprintf(stderr, "recover: impossible de charger %s\n", path);
	else if (!extract_length(&img, &len))
		fprintf(stderr, "recover: aucune longueur cachée dans %s\n", path);
	else
	{
		printf("%u\n", len);
		status = 0;
	}
	free(img.rgb);
	return (status);
}
