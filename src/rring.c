#include "rring.h"

#include <stdarg.h> /* for va_arg, va_end, va_start */

#include "rassert.h" /* for assert */
#include "rmem.h"    /* for ALLOC, NEW0, FREE */

#define T RRing_T

/*============================== Types Part ==============================*/
/*
 *
 */
/*========================================================================*/
struct T
{
    struct Node_S
    {
        struct Node_S *llink, *rlink;
        void          *val;
    }  *head;
    int length;
};

/*============================ Functions Part ============================*/
/*
 *
 */
/*========================================================================*/
T rring_new(void)
{
    T ring;

    NEW0(ring);
    ring->head = NULL;
    return ring;
}

T rring_ring(void *x, ...)
{
    T       ring = rring_new();
    va_list ap;

    va_start(ap, x);
    for (; x; x = va_arg(ap, void *)) rring_addhi(ring, x);
    va_end(ap);
    return ring;
}

void rring_free(T *ring)
{
    struct Node_S *p, *q;

    rassert(ring);
    rassert(*ring);

    if ((p = (*ring)->head) != NULL) {
        int n = (*ring)->length;
        for (; n-- > 0; p = q) {
            q = p->rlink;
            FREE(p);
        }
    }
    FREE(*ring);
}

int rring_length(T ring)
{
    rassert(ring);
    return ring->length;
}

void *rring_get(T ring, int i)
{
    struct Node_S *q;

    rassert(ring);
    rassert(i >= 0 && i < ring->length);

    {
        int n;
        q = ring->head;
        if (i <= ring->length / 2)
            for (n = i; n-- > 0;) q = q->rlink;
        else
            for (n = ring->length - i; n-- > 0;) q = q->llink;
    }
    return q->val;
}

void *rring_put(T ring, int i, void *x)
{
    struct Node_S *q;
    void          *prev;

    rassert(ring);
    rassert(i >= 0 && i < ring->length);

    {
        int n;
        q = ring->head;
        if (i <= ring->length / 2)
            for (n = i; n-- > 0;) q = q->rlink;
        else
            for (n = ring->length - i; n-- > 0;) q = q->llink;
    }
    prev   = q->val;
    q->val = x;
    return prev;
}

void *rring_add(T ring, int pos, void *x)
{
    rassert(ring);
    rassert(pos >= -ring->length && pos <= ring->length + 1);

    if (pos == 1 || pos == -ring->length)
        return rring_addlo(ring, x);
    else if (pos == 0 || pos == ring->length + 1)
        return rring_addhi(ring, x);
    else {
        struct Node_S *p, *q;
        int            i = pos < 0 ? pos + ring->length : pos - 1;
        /* find ith pos */
        int n;
        q = ring->head;
        if (i <= ring->length / 2)
            for (n = i; n-- > 0;) q = q->rlink;
        else
            for (n = ring->length - i; n-- > 0;) q = q->llink;

        NEW(p);
        /* insert */
        p->llink        = q->llink;
        q->llink->rlink = p;
        p->rlink        = q;
        q->llink        = p;
        ring->length++;
        return p->val = x;
    }
}

void *rring_addlo(T ring, void *x)
{
    rassert(ring);
    rring_addhi(ring, x);
    ring->head = ring->head->llink;
    return x;
}

void *rring_addhi(T ring, void *x)
{
    struct Node_S *p, *q;

    rassert(ring);

    NEW(p);
    if ((q = ring->head) != NULL) {
        p->llink        = q->llink;
        p->rlink        = q;
        q->llink->rlink = p;
        q->llink        = p;
    } else
        ring->head = p->llink = p->rlink = p;
    ring->length++;
    return p->val = x;
}

void *rring_remove(T ring, int i)
{
    struct Node_S *q;
    void          *x;
    int            n;

    rassert(ring);
    rassert(ring->length > 0);
    rassert(i >= 0 && i < ring->length);

    if (i <= ring->length / 2)
        for (n = i; n-- > 0;) q = q->rlink;
    else
        for (n = ring->length - i; n-- > 0;) q = q->llink;
    if (i == 0) ring->head = ring->head->rlink;
    x               = q->val;
    q->llink->rlink = q->rlink;
    q->rlink->llink = q->llink;
    FREE(q);
    if (--ring->length == 0) ring->head = NULL;
    return x;
}

void *rring_remlo(T ring)
{
    rassert(ring);
    rassert(ring->length > 0);
    ring->head = ring->head->rlink;
    return rring_remhi(ring);
}

void *rring_remhi(T ring)
{
    struct Node_S *q;
    void          *x;

    rassert(ring);
    rassert(ring->length > 0);

    q               = ring->head->llink;
    x               = q->val;
    q->rlink->llink = q->llink;
    q->llink->rlink = q->rlink;
    FREE(q);
    if (--ring->length == 0) ring->head = NULL;
    return x;
}

void rring_rotate(T ring, int n)
{
    struct Node_S *p;
    int            i;

    rassert(ring);
    rassert(n >= -ring->length && n <= ring->length);

    if (n >= 0)
        i = n % ring->length;
    else
        i = n + ring->length;

    p = ring->head;
    if (i <= ring->length / 2)
        for (n = i; n-- > 0;) p = p->rlink;
    else
        for (n = ring->length - i; n-- > 0;) p = p->llink;
    ring->head = p;
}
