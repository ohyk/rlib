#ifndef RFMT_H
#define RFMT_H
#include "rdef.h"

#include <stdarg.h>
#include <stdio.h>

#include "rexcept.h"

#define T RFmt_T

#ifndef RFMT_FLAGS_SIZE
#define RFMT_FLAGS_SIZE 256
#endif /* ifndef RFMT_FLAGS_SIZE */

#ifdef __cplusplus
extern "C" {
#endif

/*================================ Types =================================*/
/*
 * 
 */
/*========================================================================*/
typedef void (*T)(int code, RVaListBox_T *boxp, int put(int c, void *cl), void *cl,
                  unsigned char flags[RFMT_FLAGS_SIZE], int width, int precision);

/*================================ Datas =================================*/
/*
 *
 */
/*========================================================================*/
extern RLIB_EXPORT_FLAG RExcept_T const RFmtOverflow;
extern RLIB_EXPORT_FLAG char           *RFmtFlags;

/*============================== Functions ===============================*/
/*
 *
 */
/*========================================================================*/
extern RLIB_EXPORT_FLAG void rfmt_fmt(int put(int c, void *cl), void *cl, char const *fmt,
                                      ...);
extern RLIB_EXPORT_FLAG void rfmt_vfmt(int put(int c, void *cl), void *cl,
                                       char const *fmt, RVaListBox_T *boxp);
extern RLIB_EXPORT_FLAG void rfmt_print(char const *fmt, ...);
extern RLIB_EXPORT_FLAG void rfmt_fprint(FILE *stream, char const *fmt, ...);
extern RLIB_EXPORT_FLAG int  rfmt_vsfmt(char *buf, int size, char const *fmt,
                                        RVaListBox_T *boxp);
extern RLIB_EXPORT_FLAG char *rfmt_string(char const *fmt, ...);
extern RLIB_EXPORT_FLAG char *rfmt_vstring(char const *fmt, RVaListBox_T *boxp);
extern RLIB_EXPORT_FLAG T     rfmt_register(int code, T cvt);
extern RLIB_EXPORT_FLAG void rfmt_putd(char const *str, int len, int put(int c, void *cl),
                                       void *cl, unsigned char flags[RFMT_FLAGS_SIZE],
                                       int width, int precision);
extern RLIB_EXPORT_FLAG void rfmt_puts(char const *str, int len, int put(int c, void *cl),
                                       void *cl, unsigned char flags[RFMT_FLAGS_SIZE],
                                       int width, int precision);

#ifdef __cplusplus
}
#endif

#undef T

#endif /* ifndef RFMT_H */
