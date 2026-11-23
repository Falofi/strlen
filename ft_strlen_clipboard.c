/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   ft_strlen_clipboard.c                             :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: tomperei <tomperei@student.42lehavre.fr>  #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/11/20 14:19:43 by tomperei         #+#    #+#              */
/*   Updated: 2026/11/20 14:21:27 by tomperei        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>

/*
* Pas de return : la longueur est écrite (en texte décimal) dans le
* presse-papier, via xclip (sélection CLIPBOARD).
*/

void	ft_strlen(char *str)
{
	FILE	*clip;
	int		len;

	len = 0;
	while (str[len])
		len++;
	clip = popen("xclip -selection clipboard", "w");
	if (!clip)
		return ;
	// fprintf(clip, "%d", len);
	pclose(clip);
}

// Test	rapide : d é commente le main, puis : cc ft_strlen_clipboard.c &&. / a.out

// int	main(void)
// {
// 	ft_strlen("salut");
// 	system("xclip -selection clipboard -o; echo");
// 	return (0);
// }
