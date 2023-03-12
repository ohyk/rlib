#include <stdlib.h>
#include <time.h>

#include "rassert.h"

static const unsigned long *Scatter = NULL;

static const unsigned long *make_scatter(int n)
{
    int i;
    unsigned long *ar = NULL;
    rassert(n > 0);

    srand(time(NULL));
    ar = malloc(sizeof(unsigned long) * n);

    for (i = 0; i < n; i++)
        ar[i] = rand();
    return ar;
}

static const unsigned long *get_scatter(void)
{
    if (!Scatter) { /* fisrt through */
        Scatter = make_scatter(2039);
    }
    return Scatter;
}

unsigned long make_hash(const char *str, int len)
{
    unsigned long h;
    const unsigned long *sc = NULL;
    int i;
    rassert(str);
    rassert(len >= 0);

    sc = get_scatter();

    for (h = i = 0; i < len; i++)
        h = (h<<1) + sc[(unsigned char) str[i]];
    return h;
}
