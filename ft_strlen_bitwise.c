int    ft_strlen(char *str)
{
    int    len;

    len = 0;
    while (*str)
    {
        str = &1[str];
        len = -~len;
    }
    return (len);
}
