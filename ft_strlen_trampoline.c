typedef struct s_ctx    t_ctx;
typedef void    (*t_step)(t_ctx *);

struct s_ctx
{
    char    *p;
    int     len;
    t_step  next;
};

static void    check(t_ctx *c);
static void    count(t_ctx *c);

static void    check(t_ctx *c)
{
    if (*c->p)
        c->next = count;
    else
        c->next = 0;
}

static void    count(t_ctx *c)
{
    c->p++;
    c->len++;
    c->next = check;
}

int    ft_strlen(char *str)
{
    t_ctx    c;

    c.p = str;
    c.len = 0;
    c.next = check;
    while (c.next)
        c.next(&c);
    return (c.len);
}
