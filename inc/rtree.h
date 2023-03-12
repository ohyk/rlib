#ifndef RTREE_H
#define RTREE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rdef.h"

#define T RTree_T
typedef struct T *T;

extern T     rtree_new(int (*cmp)(void const *, void const *));
extern void  rtree_insert(T t, void *v);
extern void *rtree_find(T t, void const *k);
extern void *rtree_find_max(T t);
extern void *rtree_find_min(T t);

extern void rtree_map(T t, void (*apply)(void *v, void *cl), void *cl);

#undef T

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: RTREE_H */
