int    ft_strlen(char *str)
{
    if (!*str)
        return (0);
    return (ft_strlen(str + 1) + 1);
}
