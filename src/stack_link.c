#include <stddef.h>
#include <assert.h>

#include "stackrep.h"
#include "stack.h"
#include "mem.h"

#define T Stack_T

void stackrep_init(T stack)
{
    assert(stack);
    stack->count = 0;
    stack->head = NULL;
}

T stack_new(void)
{
    T s = ALLOC(sizeof(*s));
    stackrep_init(s);
    return s;
}

int stack_is_empty(T st)
{
    return (st && st->count == 0);
}

void stack_make_empty(T st)
{
    assert(st && st->count > 0);

    while (!stack_is_empty(st))
        stack_pop(st);
}

void stack_push(T st, void *e)
{
    struct elem *t;
    assert(st);

    t = ALLOC(sizeof(*t));
    t->x = e;
    t->link = st->head;
    st->count++;
    st->head = t;
}

void *stack_pop(T st)
{
    struct elem *t;
    void * x;

    assert(st);
    t = st->head;
    x = t->x;
    st->count--;
    st->head = t->link;
    FREE(t);
    return x;
}

void *stack_top(T st)
{
    return (!stack_is_empty(st)) ? st->head->x : NULL;
}

int stack_length(T st)
{
    assert(st);
    return st->count;
}
