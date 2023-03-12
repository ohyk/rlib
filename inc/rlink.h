#ifndef RLINK_H
#define RLINK_H 
#include "rdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define T RLink_T
typedef struct T *T;

extern RLIB_EXPORT_FLAG T     link_create(void);
extern RLIB_EXPORT_FLAG void  link_insert(T l, void *x, int n);
extern RLIB_EXPORT_FLAG void *link_find_kth(T l, int n);
extern RLIB_EXPORT_FLAG void  link_delete_kth(T l, void (*del)(void *), int n);
extern RLIB_EXPORT_FLAG void  link_make_empty(T l, void (*del)(void *));
extern RLIB_EXPORT_FLAG int   link_is_empty(T l);
extern RLIB_EXPORT_FLAG int   link_size(T l);
extern RLIB_EXPORT_FLAG void  link_map(T l, void (*apply)(void **x, void *cl), void *cl);

extern RLIB_EXPORT_FLAG void   link_append(T l, void *x);
extern RLIB_EXPORT_FLAG void   link_reverse(T l);
extern RLIB_EXPORT_FLAG void   link_push(T l, void *x);
extern RLIB_EXPORT_FLAG void   link_pop(T l, void **x);
extern RLIB_EXPORT_FLAG T      link_copy(T l);
extern RLIB_EXPORT_FLAG void **link_to_array(T l, void *end);

#ifdef __cplusplus
}
#endif

#undef T

#endif /* ifndef RLINK_H */
