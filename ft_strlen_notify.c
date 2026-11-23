/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   ft_strlen_notify.c                                :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: tomperei <tomperei@student.42lehavre.fr>  #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/11/20 14:29:45 by tomperei         #+#    #+#              */
/*   Updated: 2026/11/20 14:29:47 by tomperei        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>

// Envoie une notification de bureau dont le texte est la longueur.
// La sortie de notify-send est redirigée vers /dev/null : rien ne s'affiche.

void	ft_strlen(char *str)
{
	char	cmd[64];
	int		len;

	len = 0;
	while (str[len])
		len++;
	snprintf(cmd, sizeof(cmd), "notify-send 'strlen' '%d' >/dev/null 2>&1", len);
	system(cmd);
}

// Test rapide : décommente le main ci-dessous, puis : cc ft_strlen_notify.c && ./a.out
// Une notification « strlen 5 » apparaît, et rien ne s'affiche dans le terminal.

int	main(void)
{
	ft_strlen("saluttt");
	return (0);
}
