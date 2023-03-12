#ifndef RLIST_H
#define RLIST_H 
#include "rdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define T RList_T
typedef struct T *T;

struct T
{
    T     rest;
    void *first;
};

extern RLIB_EXPORT_FLAG T    rlist_append(T list, T tail);
extern RLIB_EXPORT_FLAG T    rlist_copy(T list);
extern RLIB_EXPORT_FLAG T    rlist_list(void *x, ...);
extern RLIB_EXPORT_FLAG T    rlist_pop(T list, void **x);
extern RLIB_EXPORT_FLAG T    rlist_push(T list, void *x);
extern RLIB_EXPORT_FLAG T    rlist_reverse(T list);
extern RLIB_EXPORT_FLAG int  rlist_length(T list);
extern RLIB_EXPORT_FLAG void rlist_free(T *list);
extern RLIB_EXPORT_FLAG void rlist_map(T list, void apply(void **x, void *cl), void *cl);
extern RLIB_EXPORT_FLAG void **rlist_to_array(T list, void *end);

#ifdef __cplusplus
}
#endif

#undef T

#endif /* ifndef RLIST_H */
