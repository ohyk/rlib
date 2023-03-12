#ifndef REXCEPT_H
#define REXCEPT_H

#include "rdef.h"
#include <setjmp.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* type */
#define T RExcept_T
typedef struct T
{
    const char *reson;
} T;
typedef struct RExceptFrame_T RExceptFrame_T;
struct RExceptFrame_T
{
    RExceptFrame_T *prev;
    jmp_buf         env;
    char const     *file;
    int             line;
    T const        *exception;
};

enum { RExceptEntered = 0, RExceptRaised, RExceptHandle, RExceptFinalized };

/* variable */
extern RLIB_EXPORT_FLAG RExceptFrame_T *RExceptStack;
extern RLIB_EXPORT_FLAG RExcept_T const RAssertFailed;

/* extern void rexcept_raise(T const *e, char const *file, char const *func, int line);
 */
extern RLIB_EXPORT_FLAG void rexcept_raise(T const *e, char const *file, int line);

/* macro */
/* #define RAISE(e) rexcept_raise(&(e), __FILE__, __func__, __LINE__) */
/* #define RERAISE rexcept_raise(_rexcept_frame.exception, \ */
/* _rexcept_frame.file, _rexcept_frame.func, _rexcept_frame.line) */
#define RAISE(e) rexcept_raise(&(e), __FILE__, __LINE__)
#define RERAISE \
    rexcept_raise(_rexcept_frame.exception, _rexcept_frame.file, _rexcept_frame.line)
#define RETURN                                    \
    switch (RExceptStack = RExceptStack->prev, 0) \
    default:                                      \
        return

#define TRY                                               \
    do {                                                  \
        volatile int   _rexcept_flag;                     \
        RExceptFrame_T _rexcept_frame;                    \
        _rexcept_frame.prev = RExceptStack;               \
        RExceptStack        = &_rexcept_frame;            \
        _rexcept_flag       = setjmp(_rexcept_frame.env); \
        if (_rexcept_flag == RExceptEntered) {

#define EXCEPT(e)                                                           \
    if (_rexcept_flag == RExceptEntered) RExceptStack = RExceptStack->prev; \
    }                                                                       \
    else if (_rexcept_frame.exception == &(e))                              \
    {                                                                       \
        _rexcept_flag = RExceptHandle;

#define ELSE                                                                \
    if (_rexcept_flag == RExceptEntered) RExceptStack = RExceptStack->prev; \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        _rexcept_flag = RExceptHandle;

#define FINALLY                                                             \
    if (_rexcept_flag == RExceptEntered) RExceptStack = RExceptStack->prev; \
    }                                                                       \
    {                                                                       \
        if (_rexcept_flag == RExceptEntered) _rexcept_flag = RExceptFinalized;

#define END_TRY                                                             \
    if (_rexcept_flag == RExceptEntered) RExceptStack = RExceptStack->prev; \
    }                                                                       \
    if (_rexcept_flag == RExceptRaised) RERAISE;                            \
    }                                                                       \
    while (0)

#ifdef __cplusplus
}
#endif

#undef T

#endif /* ifndef REXCEPT_H */
