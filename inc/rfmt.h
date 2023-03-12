#ifndef RFMT_H
#define RFMT_H
#include "rdef.h"

#include <stdarg.h>
#include <stdio.h>

#include "rexcept.h"

#define T RFmt_T

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*T)(int code, va_list *app, int put(int c, void *cl), void *cl,
                  unsigned char flags[256], int width, int precision);

extern RLIB_EXPORT_FLAG RExcept_T const RFmtOverflow;
extern RLIB_EXPORT_FLAG char           *fmt_flags;

extern RLIB_EXPORT_FLAG void fmt_fmt(int put(int c, void *cl), void *cl, char const *fmt,
                                     ...);
extern RLIB_EXPORT_FLAG void fmt_vfmt(int put(int c, void *cl), void *cl, char const *fmt,
                                      va_list ap);
extern RLIB_EXPORT_FLAG void fmt_print(char const *fmt, ...);
extern RLIB_EXPORT_FLAG void fmt_fprint(FILE *stream, char const *fmt, ...);
extern RLIB_EXPORT_FLAG int  fmt_vsfmt(char *buf, int size, char const *fmt, va_list ap);
extern RLIB_EXPORT_FLAG char *fmt_string(char const *fmt, ...);
extern RLIB_EXPORT_FLAG char *fmt_vstring(char const *fmt, va_list ap);
extern RLIB_EXPORT_FLAG T     fmt_register(int code, T cvt);
extern RLIB_EXPORT_FLAG void  fmt_putd(char const *str, int len, int put(int c, void *cl),
                                       void *cl, unsigned char flags[256], int width,
                                       int precision);
extern RLIB_EXPORT_FLAG void  fmt_puts(char const *str, int len, int put(int c, void *cl),
                                       void *cl, unsigned char flags[256], int width,
                                       int precision);

#ifdef __cplusplus
}
#endif

#undef T

#endif /* ifndef RFMT_H */
