#ifndef RARRAY_H
#define RARRAY_H 
#include "rdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define T RArray_T
typedef struct T *T;

extern RLIB_EXPORT_FLAG T    rarray_new(int length, int size);
extern RLIB_EXPORT_FLAG void rarray_free(T *array);

extern RLIB_EXPORT_FLAG int rarray_length(T array);
extern RLIB_EXPORT_FLAG int rarray_size(T array);

extern RLIB_EXPORT_FLAG void *rarray_get(T array, int i);
extern RLIB_EXPORT_FLAG void *rarray_put(T array, int i, void *elm);

extern RLIB_EXPORT_FLAG void rarray_resize(T array, int length);
extern RLIB_EXPORT_FLAG T    rarray_copy(T array, int length);

extern RLIB_EXPORT_FLAG void rarray_reshape(T array, int length, int size);

#ifdef __cplusplus
}
#endif

#undef T

#endif /* ifndef RARRAY_H */
