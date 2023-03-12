#include "queue.h"

#include <stdio.h>

#include "rassert.h"
#include "rmem.h"

#define T Queue_T
typedef struct T *T;

struct T
{
    int size;
    struct ele
    {
        void       *x;
        struct ele *link;
    }          *link;
    struct ele *front;
    struct ele *rear;
};

T queue_new(void)
{
    T q      = ALLOC(sizeof(*q));
    q->size  = 0;
    q->link  = NULL;
    q->front = q->link;
    q->rear  = q->link;
    return q;
}

void *queue_dequeue(T q)
{
    struct ele *t = NULL;
    void       *x;

    rassert(q);

    if (queue_is_empty(q)) return NULL;

    q->size--;
    t        = q->front;
    q->link  = q->front->link;
    q->front = q->link;
    x        = t->x;
    FREE(t);

    return x;
}

void queue_enqueue(T q, void *e)
{
    struct ele *t;
    rassert(q);

    t       = ALLOC(sizeof(*t));
    t->x    = e;
    t->link = NULL;
    if (q->link) {
        q->rear->link = t;
        q->rear       = t;
    } else { /* first through */
        q->link = q->rear = q->front = t;
    }
    q->size++;
}

int queue_is_empty(T q) { return (q && q->size == 0); }

void queue_free(T *q)
{
    T h = NULL;

    rassert(q);
    rassert(*q);

    h = *q;

    while (!queue_is_empty(h)) {
        queue_dequeue(h);
    }
    rmem_free(h, __FILE__, __LINE__);
    *q = NULL;
}
