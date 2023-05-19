#pragma once
#ifndef RVECTOR_H
#define RVECTOR_H

#include "rdef.h"

#define T RVector_T
typedef struct T *T;

#ifdef __cplusplus
extern "C" {
#endif

/* #ifdef RVECTOR_MEM */
/*============================ Rvector Hooks =============================*/
/*
 * 
 */
/*========================================================================*/
typedef struct RVectorHooks_T 
{
    void *(*allocator)(size_t nbytes, char const *file, int line);
    void (*free)(void *ptr, char const *file, int line);
    void *(*resize)(void *ptr, size_t nbytes, char const *file, int line);
} *RVectorHooks_T;
/*========================================================================*/
/*
 * Generate Rvector_T.
 *
 * If hk is NULL use mem module.
 * Client provide hk but resize is NULL, rvector use hk->allocator 
 * implement resize.
 *
 * Return:
 *      Rvector_T
 * 
 * Checked Runtime Error:
 *      size >= MEM_MAXIMUM         --> Assert_Failed
 *      if (hk) 
 *          hk->allocator == NULL   --> Assert_Failed
 *          hk->free      == NULL   --> Assert_Failed
 * 
 * Unchecked Runtime Error:
 * 
 * Exception:
 * 
 */
/*========================================================================*/
extern RLIB_EXPORT_FLAG T    rvector_new(size_t size, RVectorHooks_T hk);

/* #else */

/* extern RLIB_EXPORT_FLAG T    rvector_new(size_t size); */

/* #endif */

extern RLIB_EXPORT_FLAG void rvector_append(T v, void const *x);
extern RLIB_EXPORT_FLAG void *rvector_subtract(T v);

extern RLIB_EXPORT_FLAG void *rvector_get(T v);
extern RLIB_EXPORT_FLAG void *rvector_get_dispose(T *vp);
extern RLIB_EXPORT_FLAG void *rvector_get_dispose_shrink(T *vp);

extern RLIB_EXPORT_FLAG void   rvector_reset(T v);

extern RLIB_EXPORT_FLAG size_t rvector_length(T v);
extern RLIB_EXPORT_FLAG size_t rvector_size(T v);
extern RLIB_EXPORT_FLAG void   rvector_reverse(T v, size_t n_elem);
extern RLIB_EXPORT_FLAG size_t rvector_capacity(T v);

#ifdef RVDEBUG
void rvector_print_info(T v);
#endif

#ifdef __cplusplus
}
#endif

#undef T

#endif /* end of include guard: RVECTOR_H */
