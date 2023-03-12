#ifndef STACK_H
#define STACK_H
#include "rdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define T Stack_T
typedef struct T *T;

extern T stack_new(void);
extern int stack_is_empty(T st);
extern int stack_length(T st);
extern void stack_make_empty(T st);
extern void *stack_pop(T st);
extern void stack_push(T st, void *e);
extern void *stack_top(T st);

extern void stack_free(T *st);

#ifdef __cplusplus
}
#endif

#undef T

#endif
