#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Au lieu de la longueur, renvoie une équation a*x + b = c
// dont la solution x est exactement la longueur de la chaîne.
char	*ft_strlen(char *str)
{
	static char	eq[64];
	int			len;
	int			a;
	int			b;

	len = 0;
	while (str[len])
		len++;
	a = rand() % 9 + 1;
	b = rand() % 41 - 20;
	if (b >= 0)
		snprintf(eq, sizeof(eq), "%d*x + %d = %d", a, b, a * len + b);
	else
		snprintf(eq, sizeof(eq), "%d*x - %d = %d", a, -b, a * len + b);
	return (eq);
}

// Affiche l'équation : à toi de trouver x.
int	main(void)
{
	srand((unsigned int) time(NULL));
	printf("%s\n", ft_strlen("salut"));
	return (0);
}
