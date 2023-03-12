#include "rqueue.h"

#include <stddef.h>

#include "rmem.h"

#ifndef QUEUE_MAX_SIZE
#define QUEUE_MAX_SIZE 128
#endif /* ifndef QUEUE_MAX_SIZE */

#define T RQueue_T

struct T
{
    int    capacity;
    int    size;
    int    front;
    int    rear;
    void **var_array;
};

void rqueue_make_empty(T q)
{
    if (!q) return;
    q->size  = 0;
    q->front = 1;
    q->rear  = 0;
}

int rqueue_is_full(T q)
{
    /* mabey have someting bug, size + 1 is have full or not? */
    return (q && q->size + 1 == q->capacity);
}

T rqueue_new(void)
{
    T q          = ALLOC(sizeof(*q));
    q->capacity  = QUEUE_MAX_SIZE;
    q->var_array = ALLOC(sizeof(q->var_array[0]) * q->capacity);
    rqueue_make_empty(q);
    return q;
}

static int succ(int n, T q)
{
    /* if (++n == q->capacity) */
    /* n = 0; */
    /* return n; */
    return (++n == q->capacity) ? 0 : n;
}

void *rqueue_dequeue(T q)
{
    void *e = NULL;
    if (rqueue_is_empty(q)) return NULL;
    e        = q->var_array[q->front];
    q->front = succ(q->front, q);
    q->size--;
    return e;
}

void rqueue_enqueue(T q, void *e)
{
    if (rqueue_is_full(q)) return;
    q->size++;
    q->rear               = succ(q->rear, q);
    q->var_array[q->rear] = e;
}

void rqueue_free(T *q)
{
    if (q && *q) {
        FREE((*q)->var_array);
        FREE(*q);
    }
}

int rqueue_is_empty(T q) { return (q && q->size == 0); }
