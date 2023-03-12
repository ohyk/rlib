#ifndef RTABLE_H
#define RTABLE_H 
#include "rdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define T RTable_T
typedef struct T *T;

extern RLIB_EXPORT_FLAG T     rtable_new(int hint, int cmp(void const *x, void const *y),
                                         unsigned hash(void const *key));
extern RLIB_EXPORT_FLAG void  rtable_free(T *table);
extern RLIB_EXPORT_FLAG int   rtable_length(T table);
extern RLIB_EXPORT_FLAG void *rtable_put(T table, void const *key, void *value);
extern RLIB_EXPORT_FLAG void *rtable_get(T table, void const *key);
extern RLIB_EXPORT_FLAG void *rtable_remove(T table, void const *key);
extern RLIB_EXPORT_FLAG void  rtable_map(
     T table, void apply(void const *key, void **value, void *cl), void *cl);
extern RLIB_EXPORT_FLAG void **rtable_to_array(T table, void *end);

#ifdef __cplusplus
}
#endif

#undef T

#endif /* ifndef RTABLE_H */
