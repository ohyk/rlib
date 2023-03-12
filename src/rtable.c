#include "rtable.h"

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

#include "rassert.h"
#include "rmem.h"

#define T RTable_T

/* types */
struct T
{
    /* fields */
    int size;
    int (*cmp)(void const *x, void const *y);
    unsigned (*hash)(void const *key);
    int      length;
    unsigned timestamp;
    struct binding
    {
        struct binding *link;
        void const     *key;
        void           *value;
    } **buckets;
};

/* static functions */
static int cmpatom(void const *x, void const *y) { return x != y; }

static unsigned hashatom(void const *key) { return (uintptr_t)key >> 2; }

/* functions */
T rtable_new(int hint, int cmp(void const *x, void const *y),
            unsigned hash(void const *key))
{
    T          table;
    int        i;
    static int primes[] = {257,  257,   509,   1021,  2053,   4093,
                           8191, 16381, 32771, 65521, INT_MAX};

    rassert(hint >= 0);
    for (i = 1; primes[i - 1] < hint; i++) continue;
    table          = ALLOC(sizeof(*table) + primes[i - 1] * sizeof(table->buckets[0]));
    table->size    = primes[i - 1];
    table->cmp     = cmp ? cmp : cmpatom;
    table->hash    = hash ? hash : hashatom;
    table->buckets = (struct binding **)(table + 1);
    for (i = 0; i < table->size; i++) table->buckets[i] = NULL;
    table->length    = 0;
    table->timestamp = 0;
    return table;
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
    return p ? p->value : NULL;
}

void *rtable_put(T table, void const *key, void *value)
{
    int             i;
    struct binding *p;
    void           *prev;

    rassert(table);
    rassert(key);

    i = (*table->hash)(key) % table->size;
    for (p = table->buckets[i]; p; p = p->link)
        if ((*table->cmp)(key, p->key) == 0) break;
    if (p == NULL) {
        NEW(p);
        p->key            = key;
        p->link           = table->buckets[i];
        table->buckets[i] = p;
        table->length++;
        prev = NULL;
    } else
        prev = p->value;
    p->value = value;
    table->timestamp++;
    return prev;
}

int rtable_length(T table)
{
    rassert(table);
    return table->length;
}

/* O(MN) */
void rtable_map(T table, void apply(void const *key, void **value, void *cl), void *cl)
{
    int             i;
    unsigned        stamp;
    struct binding *p;

    rassert(table);
    rassert(apply);

    stamp = table->timestamp;
    for (i = 0; i < table->size; i++)
        for (p = table->buckets[i]; p; p = p->link) {
            apply(p->key, &p->value, cl);
            rassert(stamp == table->timestamp);
        }
}

void *rtable_remove(T table, void const *key)
{
    int              i;
    struct binding **pp;

    rassert(table);
    rassert(key);
    table->timestamp++;
    i = (*table->hash)(key) % table->size;
    for (pp = &table->buckets[i]; *pp; pp = &(*pp)->link)
        if ((*table->cmp)(key, (*pp)->key) == 0) {
            struct binding *p     = *pp;
            void           *value = p->value;
            *pp                   = p->link;
            FREE(p);
            table->length--;
            return value;
        }
    return NULL;
}

void **rtable_to_array(T table, void *end)
{
    int             i, j;
    void          **array;
    struct binding *p;

    rassert(table);
    array = ALLOC((2 * table->length + 1) * sizeof(*array));
    for (i = 0, j = 0; i < table->size; i++)
        for (p = table->buckets[i]; p; p = p->link) {
            array[j++] = (void *)p->key;
            array[j++] = p->value;
        }
    array[j] = end;
    return array;
}

void rtable_free(T *table)
{
    rassert(table && *table);

    if ((*table)->length > 0) {
        int             i;
        struct binding *p, *q;
        for (i = 0; i < (*table)->size; i++)
            for (p = (*table)->buckets[i]; p; p = q) {
                q = p->link;
                FREE(p);
            }
    }
    FREE(*table);
}
