/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   ft_strlen_codegen.c                               :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: tomperei <tomperei@student.42lehavre.fr>  #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/11/20 14:43:37 by tomperei         #+#    #+#              */
/*   Updated: 2026/11/20 14:43:37 by tomperei        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

// Ne compte rien : écrit dans gen.c un programme C qui affiche la longueur.
// Les octets de la chaîne sont écrits en octal (\ooo) pour éviter les soucis
// avec les guillemets, les antislashs et les caractères non imprimables.

void	ft_strlen(char *str)
{
	FILE	*out;
	int		i;

	out = fopen("gen.c", "w");
	if (!out)
		return ;
	fprintf(out, "#include <stdio.h>\n\n");
	fprintf(out, "int main(void)\n{\n");
	fprintf(out, "\tconst char *s = \"");
	for (i = 0; str[i]; i++)
		fprintf(out, "\\%03o", (unsigned char) str[i]);
	fprintf(out, "\";\n");
	fprintf(out, "\tint n = 0;\n\n");
	fprintf(out, "\twhile (s[n])\n\t\tn++;\n");
	fprintf(out, "\tprintf(\"%%d\\n\", n);\n");
	fprintf(out, "\treturn (0);\n}\n");
	fclose(out);
}

// Le main ne affiche rien : c'est gen.c, à compiler et lancer, qui donne la longueur.
// cc gen.c -o gen && ./gen

int	main(void)
{
	ft_strlen("salut");
	return (0);
}
