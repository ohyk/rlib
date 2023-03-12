#include "rlink.h"

#include <stddef.h>
#include <stdio.h>

#include "rassert.h"
#include "rmem.h"

#define T RLink_T

struct T
{
    int size;
    struct Elem_S
    {
        void       *x;
        struct Elem_S *next;
    } *link;
    /* struct Elem_S **rear; */
};

int rlink_is_empty(T l)
{
    rassert(l);
    return (l->size == 0);
}

int rlink_size(T l)
{
    rassert(l);
    return l->size;
}

void rlink_reverse(T l)
{
    struct Elem_S *le;
    struct Elem_S *head = NULL, *next;
    rassert(l);
    for (le = l->link; le; le = next) {
        next     = le->next;
        le->next = head;
        head     = le;
    }
    l->link = head;
}

void rlink_push(T l, void *x)
{
    struct Elem_S *e;
    rassert(l);
    NEW(e);
    e->x    = x;
    e->next = l->link;
    l->link = e;
    l->size++;
}

void rlink_pop(T l, void **x)
{
    rassert(l);
    if (rlink_is_empty(l)) return;
    if (x) {
        *x               = l->link->x;
        struct Elem_S *head = l->link->next;
        FREE(l->link);
        l->link = head;
        l->size--;
    }
}

void rlink_map(T l, void apply(void **x, void *cl), void *cl)
{
    struct Elem_S *le;
    rassert(l);
    rassert(apply);

    for (le = l->link; le; le = le->next) apply(&le->x, cl);
}

void rlink_append(T l, void *x)
{
    rassert(l);
    struct Elem_S **p = &l->link;

    struct Elem_S *newp;
    NEW(newp);
    newp->x    = x;
    newp->next = NULL;

    while (*p) p = &(*p)->next;
    *p = newp;
    l->size++;
}

T rlink_create(void)
{
    T l;
    NEW(l);
    l->size = 0;
    l->link = NULL;
    return l;
}

void rlink_insert(T l, void *x, int n)
{
    struct Elem_S *e, *le;
    int         i;
    rassert(l);
    rassert(n > 0);
    rassert(n <= rlink_size(l));

    le = l->link;
    NEW(e);
    e->x = x;
    for (i = 1; i < n && le; le = le->next, i++) continue;
    e->next  = le->next;
    le->next = e;
    /* e->next = l->link; */
    /* l->link = e; */
    l->size++;
}

void *rlink_find_kth(T l, int n)
{
    int         i;
    struct Elem_S *e;

    rassert(l);
    if (n < 1 || n > rlink_size(l)) return NULL;

    for (i = 1, e = l->link; i < n; i++, e = e->next) continue;
    return e->x;
}

void rlink_delete_kth(T l, void (*del)(void *), int n)
{
    struct Elem_S *p, *t;
    int         i;

    rassert(l);
    /* rassert(n >= 0); */
    if (n < 1 || n > rlink_size(l)) return;
    for (i = 1, p = l->link; i < n; i++) {
        t = p;
        p = p->next;
    }
    if (p == l->link)
        l->link = p->next;
    else
        t->next = p->next;
    if (del) del(p->x);
    FREE(p);
    l->size--;
}

void rlink_make_empty(T l, void (*del)(void *))
{
    rassert(l);
    while (!rlink_is_empty(l)) {
        rlink_delete_kth(l, del, 1);
    }
}

T rlink_copy(T l)
{
    T           newl;
    struct Elem_S *head, **p = &head;
    struct Elem_S *old;
    rassert(l);
    NEW(newl);
    newl->size = l->size;

    for (old = l->link; old; old = old->next) {
        NEW(*p);
        (*p)->x = old->x;
        p       = &(*p)->next;
    }
    *p         = NULL;
    newl->link = head;
    return newl;
}

void **rlink_to_array(T l, void *end)
{
    rassert(l);
    int         i, n = rlink_size(l);
    struct Elem_S *le    = l->link;
    void      **array = ALLOC((n + 1) * sizeof(*array));

    for (i = 0; i < n; i++) {
        array[i] = le->x;
        le       = le->next;
    }
    array[i] = end;
    return array;
}
