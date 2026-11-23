/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   ft_strlen_leak.c                                  :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: tomperei <tomperei@student.42lehavre.fr>  #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/11/20 14:12:56 by tomperei         #+#    #+#              */
/*   Updated: 2026/11/20 14:18:25 by tomperei        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
* Format du leak (24 octets, écrits dans le tas à chaque appel) :
*   magic  uint64  0x5354524C454E4D31 ("STRLENM1")
*   seq    uint32  numéro de l'appel (1, 2, 3...)
*   len    int32   longueur renvoyée par ft_strlen
*   check  uint32  ~len, pour écarter les faux positifs
*   pad    uint32  inutilisé
*/
#define LEAK_MAGIC 0x5354524C454E4D31ULL
#define CHUNK 4096

typedef struct s_leak
{
	uint64_t	magic;
	uint32_t	seq;
	int32_t		len;
	uint32_t	check;
	uint32_t	pad;
}	t_leak;

static uint32_t	g_seq;

int	ft_strlen(char *str)
{
	t_leak	*leak;
	int		len;

	len = 0;
	while (str[len])
		len++;
	leak = malloc(sizeof(t_leak));
	if (leak)
	{
		leak->magic = LEAK_MAGIC;
		leak->seq = ++g_seq;
		leak->len = len;
		leak->check = ~(uint32_t) len;
		leak->pad = 0;
	}
	/* Volontairement jamais libéré : c'est le leak. */
	return (len);
}

/*
* Cherche dans le tas du processus courant le leak au format ci-dessus
* et renvoie la longueur du plus récent (seq le plus grand), ou -1.
*/

int	ft_strlen_recover(void)
{
	FILE			*maps;
	char			line[256];
	unsigned long	start;
	unsigned long	end;
	unsigned long	addr;
	unsigned char	buf[CHUNK + sizeof(t_leak)];
	t_leak			l;
	uint32_t		best_seq;
	int				best_len;
	int				fd;
	int				found;
	ssize_t			n;
	size_t			off;

	maps = fopen("/proc/self/maps", "r");
	if (!maps)
		return (-1);
	found = 0;
	while (!found && fgets(line, sizeof(line), maps))
		if (strstr(line, "[heap]") && sscanf(line, "%lx-%lx", &start, &end) == 2)
		found = 1;
	fclose(maps);
	if (!found)
		return (-1);
	fd = open("/proc/self/mem", O_RDONLY);
	if (fd < 0)
		return (-1);
	best_seq = 0;
	best_len = -1;
	for (addr = start; addr < end; addr += CHUNK)
	{
		n = pread(fd, buf, sizeof(buf), (off_t) addr);
		if (n <= 0)
			break ;
		for (off = 0; off < CHUNK && off + sizeof(t_leak) <= (size_t) n; off += 8)
		{
			memcpy(&l, buf + off, sizeof(l));
			if (l.magic == LEAK_MAGIC && l.check == ~(uint32_t) l.len
					&& l.seq > best_seq)
			{
				best_seq = l.seq;
				best_len = l.len;
			}
		}
	}
	close(fd);
	return (best_len);
}

// #include <stdio.h>

// int	main(void)
// {
// 	printf("ft_strlen(\"bonjour\") = %d\n", ft_strlen("bonjour"));
// 	printf("ft_strlen_recover()   = %d\n", ft_strlen_recover());
// 	printf("ft_strlen(\"\")         = %d\n", ft_strlen(""));
// 	printf("ft_strlen_recover()   = %d\n", ft_strlen_recover());
// 	return (0);
// }
