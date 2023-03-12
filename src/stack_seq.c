#include <stddef.h>
#include "assert.h"
#include "mem.h"
#include "seq.h"
#include "stack.h"

#define T Stack_T

struct T {
    Seq_T stack;
};

T stack_new(void)
{
    T st = ALLOC(sizeof (*st));
    st->stack = seq_new(0);
    return st;
}

int stack_is_empty(T st)
{
    assert(st);
    return seq_length(st->stack) == 0;
}

void stack_make_empty(T st)
{
    assert(st);
    while (seq_length(st->stack) > 0)
        seq_remhi(st->stack);
}

void *stack_pop(T st)
{
    assert(st);
    return seq_remhi(st->stack);
}

void stack_push(T st, void *e)
{
    assert(st);
    seq_addhi(st->stack, e);
}

void *stack_top(T st)
{
    assert(st);
    return seq_length(st->stack) > 0 ? seq_get(st->stack, seq_length(st->stack) - 1) : NULL;
}

void stack_free(T *st)
{
    assert(st && *st);
    seq_free(&(*st)->stack);
    FREE(*st);
}
