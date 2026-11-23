// Bibliothèque partagée exposant ft_strlen pour le bot Discord (appelée via ctypes).
// Compilation : cc -shared -fPIC libstrlen.c -o libstrlen.so

int	ft_strlen(const char *str)
{
	int	len;

	len = 0;
	while (str[len])
		len++;
	return (len);
}
