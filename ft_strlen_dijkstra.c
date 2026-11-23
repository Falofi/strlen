#include <stdlib.h>

typedef struct s_pair
{
    int    dist;
    int    node;
}    t_pair;

typedef struct s_pq
{
    t_pair    *items;
    int       size;
    int       cap;
}    t_pq;

static void    pq_push(t_pq *q, int dist, int node)
{
    t_pair    tmp;
    int       i;
    int       parent;

    if (q->size == q->cap)
    {
        q->cap = q->cap ? q->cap * 2 : 16;
        q->items = realloc(q->items, q->cap * sizeof(t_pair));
    }
    i = q->size++;
    q->items[i].dist = dist;
    q->items[i].node = node;
    while (i > 0)
    {
        parent = (i - 1) / 2;
        if (q->items[parent].dist <= q->items[i].dist)
            break ;
        tmp = q->items[parent];
        q->items[parent] = q->items[i];
        q->items[i] = tmp;
        i = parent;
    }
}

static t_pair    pq_pop(t_pq *q)
{
    t_pair    top;
    t_pair    tmp;
    int       i;
    int       child;

    top = q->items[0];
    q->items[0] = q->items[--q->size];
    i = 0;
    while (1)
    {
        child = 2 * i + 1;
        if (child >= q->size)
            break ;
        if (child + 1 < q->size
            && q->items[child + 1].dist < q->items[child].dist)
            child++;
        if (q->items[i].dist <= q->items[child].dist)
            break ;
        tmp = q->items[i];
        q->items[i] = q->items[child];
        q->items[child] = tmp;
        i = child;
    }
    return (top);
}

/*
 * Graphe implicite : le sommet i est la position i dans la chaîne.
 * Arête i -> i+1 (coût 1) si str[i] != '\0'.
 * Raccourci i -> i+2 (coût 2) si str[i] et str[i+1] ne sont pas '\0'.
 * Les raccourcis ne coûtent pas moins cher que deux pas : la distance
 * jusqu'au sommet terminal reste la longueur de la chaîne.
 */
int    ft_strlen(char *str)
{
    t_pq      q;
    t_pair    cur;
    int       last;

    q.items = 0;
    q.size = 0;
    q.cap = 0;
    last = -1;
    pq_push(&q, 0, 0);
    while (1)
    {
        cur = pq_pop(&q);
        if (cur.node <= last)
            continue ;
        last = cur.node;
        if (str[cur.node] == '\0')
            break ;
        pq_push(&q, cur.dist + 1, cur.node + 1);
        if (str[cur.node + 1] != '\0')
            pq_push(&q, cur.dist + 2, cur.node + 2);
    }
    free(q.items);
    return (cur.dist);
}
