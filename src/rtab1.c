#include "rtable.h"

#include <limits.h>
#include <stdlib.h>

#include "rassert.h"
#include "rmem.h"

#define T RTable_T

struct T
{
    int size;
    int length;
    int timestamp;
    int (*cmp)(void const *, void const *);
    unsigned (*hash)(void const *);
    struct binding
    {
        struct binding *link;
        void const     *key;
        void           *val;
    } **buckets;
};

/* #define TEST_TABLE */
#ifdef TEST_TABLE
static int Primes[] = {11,   11,   33,    509,   1021,  2053,
                       4093, 8191, 16381, 32771, 65521, INT_MAX};
#else
static int Primes[] = {257,  257,   509,   1021,  2053,   4093,
                       8191, 16381, 32771, 65521, INT_MAX};
#endif

static double load_factor(T tab) { return ((double)tab->length / (double)tab->size); }

#define FACTOR_MAX 0.7
static void dynamic_change_rtable_size(T tab)
{
    int i;
    if (tab->size != INT_MAX && load_factor(tab) > FACTOR_MAX) {
        for (i = 1; tab->size + 1 > Primes[i - 1]; ++i) continue;
        int              newsize = Primes[i - 1];
        struct binding **newptr  = ALLOC(newsize * sizeof(*newptr));
        for (i = 0; i < newsize; ++i) newptr[i] = NULL;
        /* reset buckets */
        struct binding *p;
        struct binding *next;
        unsigned        h;
        for (i = 0; i < tab->size; ++i) {
            for (p = tab->buckets[i]; p; p = next) {
                next      = p->link;
                h         = (*tab->hash)(p->key) % newsize;
                p->link   = newptr[h];
                newptr[h] = p;
            }
        }
        FREE(tab->buckets);
        tab->size    = newsize;
        tab->buckets = newptr;
    }
}

static int cmpatom(void const *x, void const *y) { return x != y; }

static unsigned hashatom(void const *key) { return (unsigned long long)key >> 2; }

T rtable_new(int hint, int cmp(void const *, void const *), unsigned hash(void const *))
{
    T   tab;
    int i;

    rassert(hint >= 0);

    for (i = 1; Primes[i - 1] < hint; ++i) continue;
    /* tab = ALLOC(sizeof (*tab) + Primes[i-1] * sizeof (tab->buckets[0])); */
    tab            = ALLOC(sizeof(*tab));
    tab->size      = Primes[i - 1];
    tab->length    = 0;
    tab->timestamp = 0;
    tab->cmp       = cmp ? cmp : cmpatom;
    tab->hash      = hash ? hash : hashatom;
    /* tab->buckets = (struct binding **) (tab + 1); */
    tab->buckets = ALLOC(tab->size * sizeof(tab->buckets[0])); /* for dynamic change */
    for (i = 0; i < tab->size; ++i) {
        tab->buckets[i] = NULL;
    }
    return tab;
}

void rtable_free(T *table)
{
    int i;

    rassert(table && *table);

    if ((*table)->length > 0) {
        struct binding *p, *q;
        for (i = 0; i < (*table)->size; i++)
            for (p = (*table)->buckets[i]; p; p = q) {
                q = p->link;
                FREE(p);
                p = NULL;
            }
    }
    FREE(*table);
}

int rtable_length(T table)
{
    rassert(table);
    return table->length;
}

void *rtable_get(T table, void const *key)
{
    int             i;
    struct binding *p;

    rassert(table);
    rassert(key);

    i = (*table->hash)(key) % table->size;
    for (p = table->buckets[i]; p; p = p->link)
        if ((*table->cmp)(key, p->key) == 0) break;
    return p ? p->val : NULL;
}

void *rtable_put(T table, void const *key, void *val)
{
    int             i;
    struct binding *p;
    void           *prev;

    rassert(table);
    rassert(key);

    i = (*table->hash)(key) % table->size;
    for (p = table->buckets[i]; p; p = p->link) {
        if ((*table->cmp)(key, p->key) == 0) break;
    }
    if (p == NULL) {
        NEW(p);
        p->key            = key;
        p->link           = table->buckets[i];
        table->buckets[i] = p;
        table->length++;
        prev = NULL;

        dynamic_change_rtable_size(table);
    } else
        prev = p->val;
    p->val = val;
    table->timestamp++;
    return prev;
}

void *rtable_remove(T table, void const *key)
{
    int              i;
    struct binding **pp;

    rassert(table);
    rassert(key);

    i = (*table->hash)(key) % table->size;
    for (pp = &table->buckets[i]; *pp; pp = &(*pp)->link)
        if ((*table->cmp)(key, (*pp)->key) == 0) {
            struct binding *p   = *pp;
            void           *val = p->val;
            *pp                 = p->link;
            FREE(p);
            table->length--;
            return val;
        }
    return NULL;
}

void rtable_map(T table, void apply(void const *key, void **val, void *cl), void *cl)
{
    int             i;
    struct binding *p;
    int             stamp;

    rassert(table);
    rassert(apply);

    stamp = table->timestamp;
    for (i = 0; i < table->size; i++)
        for (p = table->buckets[i]; p; p = p->link) {
            apply(p->key, &p->val, cl);
            rassert(stamp == table->timestamp);
        }
}

void **rtable_to_array(T table, void *end)
{
    rassert(table);

    int             i, j;
    struct binding *p;
    void          **ar = ALLOC((table->length + 1) * sizeof(*ar));
    for (i = j = 0; i < table->size; i++)
        for (p = table->buckets[i]; p; p++) {
            ar[j++] = (void *)p->key;
            ar[j++] = p->val;
        }
    ar[j] = end;
    return ar;
}

/* #define TEST_TABLE */
#ifdef TEST_TABLE
#include "rlog.h"
void print_rtable_info(T table)
{
    rassert(table);

    rlog_message("<--- [%p] Info --->", table);
    rlog_message("hash: %p", table->hash);
    rlog_message("cmp: %p", table->cmp);
    rlog_message("size: %d", table->size);
    rlog_message("length: %d", table->length);
    rlog_message("factor: %f", load_factor(table));
    rlog_message("buckets: %p", table->buckets);
    rlog_message("<--- [%p] END  --->", table);
}
#endif
