#ifndef RQUEUE_H
#define RQUEUE_H 
#include "rdef.h"

#ifdef __cplusplus
extern "C" {
#endif

// typedef void *Queue_Element_t;

#define T RQueue_T
typedef struct T *T;

extern RLIB_EXPORT_FLAG T     rqueue_new(void);
extern RLIB_EXPORT_FLAG void *rqueue_dequeue(T q);
extern RLIB_EXPORT_FLAG void  rqueue_enqueue(T q, void *e);
extern RLIB_EXPORT_FLAG int   rqueue_is_empty(T q);

extern RLIB_EXPORT_FLAG void rqueue_free(T *q);

#ifdef __cplusplus
}
#endif

#undef T

#endif /* ifndef RQUEUE_H */
