#ifndef RRING_H
#define RRING_H 
#include "rdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define T RRing_T
typedef struct T *T;

extern RLIB_EXPORT_FLAG T rring_new(void);
extern RLIB_EXPORT_FLAG T rring_ring(void *x, ...);

extern RLIB_EXPORT_FLAG void rring_free(T *ring);
extern RLIB_EXPORT_FLAG int  rring_length(T ring);

extern RLIB_EXPORT_FLAG void *rring_get(T ring, int i);
extern RLIB_EXPORT_FLAG void *rring_put(T ring, int i, void *x);

extern RLIB_EXPORT_FLAG void *rring_add(T ring, int pos, void *x);
extern RLIB_EXPORT_FLAG void *rring_addlo(T ring, void *x);
extern RLIB_EXPORT_FLAG void *rring_addhi(T ring, void *x);

extern RLIB_EXPORT_FLAG void *rring_remove(T ring, int i);
extern RLIB_EXPORT_FLAG void *rring_remlo(T ring);
extern RLIB_EXPORT_FLAG void *rring_remhi(T ring);

extern RLIB_EXPORT_FLAG void rring_rotate(T ring, int n);

#ifdef __cplusplus
}
#endif

#undef T 

#endif /* ifndef RRING_H */
