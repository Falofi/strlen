/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   ft_strlen_unroll.c                                :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: tomperei <tomperei@student.42lehavre.fr>  #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/11/23 10:13:16 by tomperei         #+#    #+#              */
/*   Updated: 2026/11/23 10:13:17 by tomperei        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

int	ft_strlen(char *str)
{
	char	*p;

	p = str;
	while (p[0] && p[1] && p[2] && p[3])
		p += 4;
	if (!p[0])
		return (p - str);
	if (!p[1])
		return (p + 1 - str);
	if (!p[2])
		return (p + 2 - str);
	return (p + 3 - str);
}
