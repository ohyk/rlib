#ifndef RSEQ_H
#define RSEQ_H 
#include "rdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define T RSeq_T
typedef struct T *T;

extern RLIB_EXPORT_FLAG T rseq_new(int hint);
extern RLIB_EXPORT_FLAG T rseq_seq(void *x, ...);

extern RLIB_EXPORT_FLAG void rseq_free(T *seq);

extern RLIB_EXPORT_FLAG int rseq_length(T seq);

extern RLIB_EXPORT_FLAG void *rseq_get(T seq, int i);
extern RLIB_EXPORT_FLAG void *rseq_put(T seq, int i, void *x);

extern RLIB_EXPORT_FLAG void *rseq_addlo(T seq, void *x);
extern RLIB_EXPORT_FLAG void *rseq_addhi(T seq, void *x);

extern RLIB_EXPORT_FLAG void *rseq_remlo(T seq);
extern RLIB_EXPORT_FLAG void *rseq_remhi(T seq);

#ifdef __cplusplus
}
#endif

#undef T

#endif /* ifndef RSEQ_H */
