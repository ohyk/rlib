#include "rlist.h"

#include <stdarg.h>
#include <stddef.h>

#include "rassert.h"
#include "rmem.h"

#define T RList_T

T rlist_append(T list, T tail)
{
    T *p = &list;

    while (*p) p = &(*p)->rest;
    *p = tail;
    return list;
}

T rlist_copy(T list)
{
    T head, *p = &head;

    for (; list; list = list->rest) {
        NEW(*p);
        (*p)->first = list->first;
        p           = &(*p)->rest;
    }
    *p = NULL;
    return head;
}

T rlist_list(void *x, ...)
{
    va_list ap;
    T       list, *p = &list;

    va_start(ap, x);
    for (; x; x = va_arg(ap, void *)) {
        NEW(*p);
        (*p)->first = x;
        p           = &(*p)->rest;
    }
    *p = NULL;
    va_end(ap);
    return list;
}

T rlist_pop(T list, void **x)
{
    if (!list) return list;
    if (x) *x = list->first;
    T head = list->rest;
    FREE(list);
    return head;
}

T rlist_push(T list, void *x)
{
    T p;

    NEW(p);
    p->first = x;
    p->rest  = list;
    return p;
}

T rlist_reverse(T list)
{
    T head = NULL, next;

    for (; list; list = next) {
        next       = list->rest;
        list->rest = head;
        head       = list;
    }
    return head;
}

int rlist_length(T list)
{
    int i = 0;
    while (list) {
        i++;
        list = list->rest;
    }
    return i;
}

void rlist_free(T *list)
{
    T next;

    rassert(list);
    for (; *list; *list = next) {
        next = (*list)->rest;
        FREE(*list);
    }
}

void rlist_map(T list, void apply(void **x, void *cl), void *cl)
{
    rassert(apply);
    for (; list; list = list->rest) apply(&list->first, cl);
}

void **rlist_to_array(T list, void *end)
{
    int    i, n = rlist_length(list);
    void **array = ALLOC((n + 1) * sizeof(*array));

    for (i = 0; i < n; i++) {
        array[i] = list->first;
        list     = list->rest;
    }
    array[i] = end;
    return array;
}
