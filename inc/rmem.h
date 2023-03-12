/*====================================================================*/
/*
 * Copyright (C) 2018-2021 Richard Guo
 * Created on 2018/02/25
 */
/*====================================================================*/
#ifndef RMEM_H
#define RMEM_H 
#include "rdef.h"
#include "rexcept.h"

#ifdef __cplusplus
extern "C" {
#endif

/* exported exceptions */
extern RLIB_EXPORT_FLAG RExcept_T const RMemFailed;

/* exported functions */
extern RLIB_EXPORT_FLAG void *rmem_alloc(size_t nbytes, char const *file, int line);
extern RLIB_EXPORT_FLAG void *rmem_calloc(size_t count, size_t nbytes, char const *file,
                                          int line);
extern RLIB_EXPORT_FLAG void  rmem_free(void *ptr, char const *file, int line);
extern RLIB_EXPORT_FLAG void *rmem_resize(void *ptr, size_t nbytes, char const *file,
                                          int line);

#ifdef MEM_CHK
extern RLIB_EXPORT_FLAG void rmem_leak(void (*apply)(void *ptr, size_t size,
                                                     char const *file, int line,
                                                     void *cl),
                                       void *cl);
extern RLIB_EXPORT_FLAG void rmem_print_allocated(void);
extern RLIB_EXPORT_FLAG void rmem_print_freed(void);
extern RLIB_EXPORT_FLAG void rmem_atexit_leak(void);
#endif

/* exported macros */
#define ALLOC(nbytes) rmem_alloc((nbytes), __FILE__, __LINE__)
#define CALLOC(count, nbytes) rmem_calloc((count), (nbytes), __FILE__, __LINE__)
#define NEW(p) ((p) = ALLOC((long)sizeof *(p)))
#define NEW0(p) ((p) = CALLOC(1, (long)sizeof *(p)))
#define FREE(ptr) ((void)(rmem_free((ptr), __FILE__, __LINE__), (ptr) = 0))
#define RESIZE(ptr, nbytes) ((ptr) = rmem_resize((ptr), (nbytes), __FILE__, __LINE__))

#ifdef __cplusplus
}
#endif

#endif /* ifndef RMEM_H */
