#ifndef RARRAYPTR_H
#define RARRAYPTR_H 
#include "rdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define T RArray_T
typedef struct T *T;

extern RLIB_EXPORT_FLAG T    rarrayptr_new(int length);
extern RLIB_EXPORT_FLAG void rarrayptr_free(T *array);

extern RLIB_EXPORT_FLAG void *rarrayptr_get(T array, int i);
extern RLIB_EXPORT_FLAG void *rarrayptr_put(T array, int i, void *eptr);

extern RLIB_EXPORT_FLAG int rarrayptr_length(T array);

extern RLIB_EXPORT_FLAG T    rarrayptr_copy(T array, int length);
extern RLIB_EXPORT_FLAG void rarrayptr_resize(T array, int length);

#ifdef __cplusplus
}
#endif

#undef T

#endif /* ifndef RARRAYPTR_H */
