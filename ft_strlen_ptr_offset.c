int    ft_strlen(char *str)
{
char    *p;

p = str;
while (p[p - p])
    p = p - -1;
return (p - str);
}
