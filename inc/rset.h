#ifndef RSET_H
#define RSET_H 
#include "rdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define T RSet_T
typedef struct T *T;

extern RLIB_EXPORT_FLAG T    rset_new(int hint, int cmp(void const *x, void const *y),
                                      unsigned hash(void const *x));
extern RLIB_EXPORT_FLAG void rset_free(T *set);

extern RLIB_EXPORT_FLAG int   rset_length(T set);
extern RLIB_EXPORT_FLAG int   rset_member(T set, void const *member);
extern RLIB_EXPORT_FLAG void  rset_put(T set, void const *member);
extern RLIB_EXPORT_FLAG void *rset_remove(T set, void const *member);

extern RLIB_EXPORT_FLAG void   rset_map(T set, void apply(void const *member, void *cl),
                                        void *cl);
extern RLIB_EXPORT_FLAG void **rset_to_array(T set, void *end);

/* s + t */
extern RLIB_EXPORT_FLAG T rset_union(T s, T t);
/* s * t */
extern RLIB_EXPORT_FLAG T rset_inter(T s, T t);
/* s - t */
extern RLIB_EXPORT_FLAG T rset_minus(T s, T t);
/* s / t */
extern RLIB_EXPORT_FLAG T rset_diff(T s, T t);

#ifdef __cplusplus
}
#endif

#undef T

#endif /* ifndef RSET_H */
