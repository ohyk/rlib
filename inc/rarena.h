/*====================================================================*/
/*
 * Copyright (C) 2018-2021 Richard Guo
 * Created on 2018/7/13
 */
/*====================================================================*/
/*====================================================================*/
/*
 * RArena --- memory pool
 *
 * Exported Type:
 *      - RArena_T
 *
 * Checked Runtime Error:
 *      - arena is NULL     --> RAssertFailed
 *      - ap && *ap is NULL --> RAssertFailed
 *      - if defined MEM_MAXIMUM && count*nbytes > MEM_MAXIMUM 
 *                          --> RAssertFailed
 *
 * Unchecked Runtime Error: None
 *
 * Exception: None
 *
 */
/*====================================================================*/
#ifndef RARENA_H
#define RARENA_H 
#include "rdef.h"
#include "rexcept.h"

#ifdef __cplusplus
extern "C" {
#endif

#define T RArena_T
typedef struct T *T;

extern RLIB_EXPORT_FLAG RExcept_T const RArenaNewFailed;
extern RLIB_EXPORT_FLAG RExcept_T const RArenaFailed;

extern RLIB_EXPORT_FLAG T    rarena_new(void);
extern RLIB_EXPORT_FLAG void rarena_dispose(T *ap);

extern RLIB_EXPORT_FLAG void *rarena_alloc(T arena, size_t nbytes, char const *file,
                                           int line);
extern RLIB_EXPORT_FLAG void *rarena_calloc(T arena, size_t count, size_t nbytes,
                                            char const *file, int line);
extern RLIB_EXPORT_FLAG void  rarena_free(T arena);

#ifdef RARENA_DEBUG
void rarena_print(T arena);
void rarena_print_freechunks(void);
#endif

#define RARENA_ALLOC(a, n) rarena_alloc((a), (n), __FILE__, __LINE__)
#define RARENA_CALLOC(a, c, n) rarena_calloc((a), (c), (n), __FILE__, __LINE__)
#define RARENA_NEW(a, p) ((p) = RARENA_ALLOC((a), sizeof(*(p))))

#ifdef __cplusplus
}
#endif

#undef T

#endif /* ifndef RARENA_H */
