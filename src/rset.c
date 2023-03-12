#include "rset.h"

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

#include "rarith.h"
#include "rassert.h"
#include "rmem.h"

#define T RSet_T

/* types */
struct T
{
    int      length;
    unsigned timestamp;
    int (*cmp)(void const *x, void const *y);
    unsigned (*hash)(void const *x);
    int size;
    struct Member_S
    {
        struct Member_S *link;
        void const      *member;
    } **buckets;
};

/* static functions */
static int cmpatom(void const *x, void const *y) { return x != y; }

static unsigned hashatom(void const *x) { return (uintptr_t)x >> 2; }

static T copy(T t, int hint)
{
    T set;

    rassert(t);
    set = rset_new(hint, t->cmp, t->hash);
    {
        int              i;
        struct Member_S *q;
        for (i = 0; i < t->size; i++)
            for (q = t->buckets[i]; q; q = q->link) {
                struct Member_S *p;
                void const      *member = q->member;
                int              i      = (*set->hash)(member) % set->size;
                NEW(p);
                p->member       = member;
                p->link         = set->buckets[i];
                set->buckets[i] = p;
                set->length++;
            }
    }
    return set;
}

/* functions */
T rset_new(int hint, int cmp(void const *x, void const *y), unsigned hash(void const *x))
{
    T          set;
    int        i;
    static int primes[] = {509,  509,   1021,  2053,  4093,
                           8191, 16381, 32771, 65521, INT_MAX};

    rassert(hint >= 0);
    for (i = 1; primes[i] < hint; i++) continue;
    set            = ALLOC(sizeof(*set) + (primes[i - 1] * sizeof(set->buckets[0])));
    set->size      = primes[i - 1];
    set->length    = 0;
    set->timestamp = 0;
    set->cmp       = cmp ? cmp : cmpatom;
    set->hash      = hash ? hash : hashatom;
    set->buckets   = (struct Member_S **)(set + 1);
    for (i = 0; i < set->size; i++) set->buckets[i] = NULL;
    return set;
}

int rset_member(T set, void const *member)
{
    int              i;
    struct Member_S *p;

    rassert(set);
    rassert(member);
    i = (*set->hash)(member) % set->size;
    for (p = set->buckets[i]; p; p = p->link)
        if ((*set->cmp)(member, p->member) == 0) break;
    return p != NULL;
}

void rset_put(T set, void const *member)
{
    int              i;
    struct Member_S *p;

    rassert(set);
    rassert(member);
    i = (*set->hash)(member) % set->size;
    for (p = set->buckets[i]; p; p = p->link)
        if ((*set->cmp)(member, p->member) == 0) break;
    if (p == NULL) {
        NEW(p);
        p->member       = member;
        p->link         = set->buckets[i];
        set->buckets[i] = p;
        set->length++;
    } else
        p->member = member;
    set->timestamp++;
}

void *rset_remove(T set, void const *member)
{
    int               i;
    struct Member_S **pp;

    rassert(set);
    rassert(member);
    i = (*set->hash)(member) % set->size;
    for (pp = &set->buckets[i]; *pp; pp = &(*pp)->link) {
        if ((*set->cmp)(member, (*pp)->member) == 0) {
            struct Member_S *p = *pp;
            *pp                = p->link;
            member             = p->member;
            FREE(p);
            set->length--;
            return (void *)member;
        }
    }
    return NULL;
}

int rset_length(T set)
{
    rassert(set);
    return set->length;
}

void rset_free(T *set)
{
    rassert(set && *set);
    if ((*set)->length > 0) {
        int              i;
        struct Member_S *p, *q;
        for (i = 0; i < (*set)->size; i++)
            for (p = (*set)->buckets[i]; p; p = q) {
                q = p->link;
                FREE(p);
            }
    }
    FREE(*set);
}

void rset_map(T set, void apply(void const *member, void *cl), void *cl)
{
    rassert(set);
    rassert(apply);
    if (set->length > 0) {
        int              i;
        unsigned         stamp = set->timestamp;
        struct Member_S *p;
        for (i = 0; i < set->size; i++)
            for (p = set->buckets[i]; p; p = p->link) {
                apply(p->member, cl);
                rassert(set->timestamp == stamp);
            }
    }
}

void **rset_to_array(T set, void *end)
{
    int              i, j;
    void           **array;
    struct Member_S *p;

    rassert(set);
    array = ALLOC((set->length + 1) * sizeof(*array));
    for (i = 0, j = 0; i < set->size; i++)
        for (p = set->buckets[i]; p; p = p->link) array[j++] = (void *)p->member;
    array[j] = end;
    return array;
}

T rset_union(T s, T t)
{
    if (s == NULL) {
        rassert(t);
        return copy(t, t->size);
    } else if (t == NULL)
        return copy(s, s->size);
    else {
        T set = copy(s, rarith_max(s->size, t->size));
        rassert(s->cmp == t->cmp && s->hash == t->hash);
        {
            int              i;
            struct Member_S *q;
            for (i = 0; i < t->size; i++)
                for (q = t->buckets[i]; q; q = q->link) rset_put(set, q->member);
        }
        return set;
    }
}

T rset_inter(T s, T t)
{
    if (s == NULL) {
        rassert(t);
        return rset_new(t->size, t->cmp, t->hash);
    } else if (t == NULL)
        return rset_new(s->size, s->cmp, s->hash);
    /* else if (s->length < t->length) */
    /* return rset_inter(t, s); */
    else {
        if (s->length < t->length) {
            T tmp = s;
            s     = t;
            t     = tmp;
        }
        T set = rset_new(rarith_min(s->size, t->size), s->cmp, s->hash);
        rassert(s->cmp == t->cmp && s->hash == t->hash);
        {
            int              i;
            struct Member_S *q;
            for (i = 0; i < t->size; i++)
                for (q = t->buckets[i]; q; q = q->link)
                    if (rset_member(s, q->member)) {
                        struct Member_S *p;
                        void const      *member = q->member;
                        int              i      = (*set->hash)(member) % set->size;
                        NEW(p);
                        p->member       = member;
                        p->link         = set->buckets[i];
                        set->buckets[i] = p;
                        set->length++;
                    }
        }
        return set;
    }
}

T rset_minus(T t, T s)
{
    if (t == NULL) {
        rassert(s);
        return rset_new(s->size, s->cmp, s->hash);
    } else if (s == NULL)
        return copy(t, t->size);
    else {
        T set = rset_new(rarith_min(s->size, t->size), s->cmp, s->hash);
        rassert(s->cmp == t->cmp && s->hash == t->hash);
        {
            int              i;
            struct Member_S *q;
            for (i = 0; i < t->size; i++)
                for (q = t->buckets[i]; q; q = q->link)
                    if (!rset_member(s, q->member)) {
                        struct Member_S *p;
                        void const      *member = q->member;
                        int              i      = (*set->hash)(member) % set->size;
                        NEW(p);
                        p->member       = member;
                        p->link         = set->buckets[i];
                        set->buckets[i] = p;
                        set->length++;
                    }
        }
        return set;
    }
}

T rset_diff(T s, T t)
{
    if (s == NULL) {
        rassert(t);
        return copy(t, t->size);
    } else if (t == NULL)
        return copy(s, s->size);
    else {
        T set = rset_new(rarith_min(s->size, t->size), s->cmp, s->hash);
        rassert(s->cmp == t->cmp && s->hash == t->hash);
        {
            int              i;
            struct Member_S *q;
            for (i = 0; i < t->size; i++)
                for (q = t->buckets[i]; q; q = q->link)
                    if (!rset_member(s, q->member)) {
                        struct Member_S *p;
                        void const      *member = q->member;
                        int              i      = (*set->hash)(member) % set->size;
                        NEW(p);
                        p->member       = member;
                        p->link         = set->buckets[i];
                        set->buckets[i] = p;
                        set->length++;
                    }
        }
        {
            T tmp = s;
            s     = t;
            t     = tmp;
        }
        {
            int              i;
            struct Member_S *q;
            for (i = 0; i < t->size; i++)
                for (q = t->buckets[i]; q; q = q->link)
                    if (!rset_member(s, q->member)) {
                        struct Member_S *p;
                        void const      *member = q->member;
                        int              i      = (*set->hash)(member) % set->size;
                        NEW(p);
                        p->member       = member;
                        p->link         = set->buckets[i];
                        set->buckets[i] = p;
                        set->length++;
                    }
        }
        return set;
    }
}
