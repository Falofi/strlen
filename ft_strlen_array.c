int    ft_strlen(char *str)
{
    char    *tab[2];

    tab[0] = str;
    tab[1] = str;
    while (*tab[1])
        tab[1]++;
    return (tab[1] - tab[0]);
}
