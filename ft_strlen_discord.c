/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   ft_strlen_discord.c                               :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: tomperei <tomperei@student.42lehavre.fr>  #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/11/20 14:51:35 by tomperei         #+#    #+#              */
/*   Updated: 2026/11/20 14:55:01 by tomperei        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

// URL du webhook Discord en dur : quiconque a ce fichier peut poster dans le salon.
#define WEBHOOK_URL "https://discordapp.com/api/webhooks/1573333794583347302/zlMpXEaxxJMipbHJBQlOAMq7yGS5DyIlJbNT1uEAQH6HlxVP7pPfcEE5a8CksIji48CN"

// Envoie la longueur de la chaîne sur un webhook Discord, sans rien afficher en cas de succès.
// Les erreurs de curl sont affichées sur stderr.
// curl est lancé directement (execlp, sans shell) : l'URL n'est jamais interprétée.

void	ft_strlen(char *str)
{
	char	content[64];
	char	*url;
	int		len;
	pid_t	pid;
	int		devnull;

	len = 0;
	while (str[len])
		len++;
	url = WEBHOOK_URL;
	snprintf(content, sizeof(content), "{\"content\": \"%d\"}", len);
	pid = fork();
	if (pid == 0)
	{
		devnull = open("/dev/null", O_WRONLY);
		dup2(devnull, 1);
		execlp("curl", "curl", "-sS", "-f", "-X", "POST",
			"-H", "Content-Type: application/json",
		"-d", content, url, (char *) NULL);
		_exit(127);
	}
	if (pid > 0)
		waitpid(pid, NULL, 0);
}

// Le main envoie la longueur de « salut » sur Discord.
// cc ft_strlen_discord.c && ./a.out

int	main(void)
{
	ft_strlen("karmine est la meilleure équipe du monde");
	return (0);
}
