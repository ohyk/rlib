#include "stack.h"

#include <limits.h>
#include <stddef.h>
#include <stdio.h>

#include "rexcept.h"
#include "rmem.h"

#define T Stack_T
#define STACK_MAX_SIZE 512
#define STACK_ERROR (-1)
#define STACK_EXPAND_NUMBER 128

typedef struct T *T;
struct T
{
    int    capacity;
    int    offset;
    void **var_array;
};

static RExcept_T StackExpandFailed = {"Stack Expand Failed"};

static int expand(T s)
{
    int newcap;
    if (s->capacity < INT_MAX) {
        newcap      = s->capacity + STACK_EXPAND_NUMBER;
        s->capacity = newcap;
        RESIZE(s->var_array, newcap * sizeof(s->var_array[0]));
    } else
        newcap = STACK_ERROR;
    return newcap;
}

int stack_is_empty(Stack_T st) { return (st && st->offset == -1); }

Stack_T stack_new(void)
{
    Stack_T s    = ALLOC(sizeof(*s));
    s->var_array = ALLOC(sizeof(s->var_array[0]) * STACK_MAX_SIZE);
    s->capacity  = STACK_MAX_SIZE;
    s->offset    = -1;
    return s;
}

void stack_make_empty(Stack_T st)
{
    if (!st) return;
    st->offset = -1;
}

void *stack_pop(Stack_T st)
{
    /* void * e = NULL; */
    if (!st || st->offset == -1) return NULL;
    return st->var_array[st->offset--];
}

void stack_push(Stack_T st, void *e)
{
    if (!st) return;
    if (st->offset + 1 >= st->capacity && expand(st) == STACK_ERROR)
        RAISE(StackExpandFailed); /* full and expand failed */

    st->var_array[++st->offset] = e;
}

void *stack_top(Stack_T st)
{
    return (stack_is_empty(st)) ? NULL : st->var_array[st->offset];
}

void stack_free(T *st)
{
    if (st) {
        FREE((*st)->var_array);
        FREE(*st);
    }
}

void stack_print(T st)
{
    printf("--- stack ---\n");
    printf("capacity: %d\n", st->capacity);
    printf("offset:%d\n", st->offset);
    printf("var_arr: %p\n", st->var_array);
    printf("--- end ---\n");
}
