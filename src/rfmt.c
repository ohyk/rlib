#include "rfmt.h"

#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rassert.h"
#include "rexcept.h"
#include "rmem.h"

#define T RFmt_T

struct Buf_S
{
    char *buf;
    char *bp;
    int   size;
};

#define PAD(n, c)                      \
    do {                               \
        int nn = (n);                  \
        while (nn-- > 0) put((c), cl); \
    } while (0)

/* static functions */
static void cvt_s(int code, RVaListBox_T *boxp, int put(int c, void *cl), void *cl,
                  unsigned char flags[], int width, int precision)
{
    char *str = va_arg(boxp->ap, char *);
    rassert(str);
    rfmt_puts(str, strlen(str), put, cl, flags, width, precision);
}

static void cvt_d(int code, RVaListBox_T *boxp, int put(int c, void *cl), void *cl,
                  unsigned char flags[], int width, int precision)
{
    int      val = va_arg(boxp->ap, int);
    unsigned m;
    char     buf[43];
    char    *p = buf + sizeof(buf);

    if (val == INT_MIN)
        m = INT_MAX + 1U;
    else if (val < 0)
        m = -val;
    else
        m = val;

    do {
        *--p = m % 10 + '0';
    } while ((m /= 10) > 0);

    if (val < 0) *--p = '-';
    rfmt_putd(p, (buf + sizeof(buf)) - p, put, cl, flags, width, precision);
}

static void cvt_u(int code, RVaListBox_T *boxp, int put(int c, void *cl), void *cl,
                  unsigned char flags[], int width, int precision)
{
    unsigned m = va_arg(boxp->ap, unsigned);
    char     buf[43];
    char    *p = buf + sizeof(buf);

    do {
        *--p = m % 10 + '0';
    } while ((m /= 10) > 0);
    rfmt_putd(p, (buf + sizeof(buf)) - p, put, cl, flags, width, precision);
}

static void cvt_o(int code, RVaListBox_T *boxp, int put(int c, void *cl), void *cl,
                  unsigned char flags[], int width, int precision)
{
    unsigned m = va_arg(boxp->ap, unsigned);
    char     buf[43];
    char    *p = buf + sizeof(buf);

    do {
        *--p = (m & 0x7) + '0';
    } while ((m >>= 3) != 0);
    rfmt_putd(p, (buf + sizeof(buf)) - p, put, cl, flags, width, precision);
}

static void cvt_x(int code, RVaListBox_T *boxp, int put(int c, void *cl), void *cl,
                  unsigned char flags[], int width, int precision)
{
    unsigned m = va_arg(boxp->ap, unsigned);
    char     buf[43];
    char    *p = buf + sizeof(buf);

    do {
        *--p = "0123456789abcdef"[m & 0xf];
    } while ((m >>= 4) != 0);
    rfmt_putd(p, (buf + sizeof(buf)) - p, put, cl, flags, width, precision);
}

static void cvt_p(int code, RVaListBox_T *boxp, int put(int c, void *cl), void *cl,
                  unsigned char flags[], int width, int precision)
{
    uintptr_t m = (uintptr_t)va_arg(boxp->ap, void *);
    char      buf[43];
    char     *p = buf + sizeof(buf);

    do {
        *--p = "0123456789abcdef"[m & 0xf];
    } while ((m >>= 4) != 0);
    rfmt_putd(p, (buf + sizeof(buf)) - p, put, cl, flags, width, precision);
}

static void cvt_c(int code, RVaListBox_T *boxp, int put(int c, void *cl), void *cl,
                  unsigned char flags[], int width, int precision)
{
    if (width == INT_MAX) width = 0;
    if (width < 0) {
        flags['-'] = 1;
        width      = -width;
    }
    if (!flags['-']) PAD(width - 1, ' ');
    put((unsigned char)va_arg(boxp->ap, int), cl);
    if (flags['-']) PAD(width - 1, ' ');
}

static void cvt_f(int code, RVaListBox_T *boxp, int put(int c, void *cl), void *cl,
                  unsigned char flags[], int width, int precision)
{
    char buf[DBL_MAX_10_EXP + 1 + 1 + 99 + 1];
    if (precision < 0) precision = 6;
    if (code == 'g' && precision == 0) precision = 1;
    {
        static char fmt[] = "%.dd?";
        rassert(precision <= 99);
        fmt[4] = code;
        fmt[3] = precision % 10 + '0';
        fmt[2] = (precision / 10) % 10 + '0';
        sprintf(buf, fmt, va_arg(boxp->ap, double));
    }
    rfmt_putd(buf, strlen(buf), put, cl, flags, width, precision);
}

/* data */
char           *FmtFlags     = "-+ 0";
const RExcept_T RFmtOverflow = {"Formatting Overflow"};

static T ConvertTable[256] = {
    /*   0-  7 */ 0,     0, 0, 0,     0,     0,     0,     0,
    /*   8- 15 */ 0,     0, 0, 0,     0,     0,     0,     0,
    /*  16- 23 */ 0,     0, 0, 0,     0,     0,     0,     0,
    /*  24- 31 */ 0,     0, 0, 0,     0,     0,     0,     0,
    /*  32- 39 */ 0,     0, 0, 0,     0,     0,     0,     0,
    /*  40- 47 */ 0,     0, 0, 0,     0,     0,     0,     0,
    /*  48- 55 */ 0,     0, 0, 0,     0,     0,     0,     0,
    /*  56- 63 */ 0,     0, 0, 0,     0,     0,     0,     0,
    /*  64- 71 */ 0,     0, 0, 0,     0,     0,     0,     0,
    /*  72- 79 */ 0,     0, 0, 0,     0,     0,     0,     0,
    /*  80- 87 */ 0,     0, 0, 0,     0,     0,     0,     0,
    /*  88- 95 */ 0,     0, 0, 0,     0,     0,     0,     0,
    /*  96-103 */ 0,     0, 0, cvt_c, cvt_d, cvt_f, cvt_f, cvt_f,
    /* 104-111 */ 0,     0, 0, 0,     0,     0,     0,     cvt_o,
    /* 112-119 */ cvt_p, 0, 0, cvt_s, 0,     cvt_u, 0,     0,
    /* 120-127 */ cvt_x, 0, 0, 0,     0,     0,     0,     0};

/* functions */
void rfmt_puts(char const *str, int len, int put(int c, void *cl), void *cl,
               unsigned char flags[256], int width, int precision)
{
    rassert(str);
    rassert(len >= 0);
    rassert(flags);

    if (width == INT_MIN) width = 0;
    if (width < 0) {
        flags['-'] = 1;
        width      = -width;
    }
    if (precision >= 0) flags['0'] = 0;
    if (precision >= 0 && precision < len) len = precision;
    if (!flags['-']) PAD(width - len, ' ');

    {
        for (int i = 0; i < len; i++) put((unsigned char)*str++, cl);
    }

    if (flags['-']) PAD(width - len, ' ');
}

void rfmt_fmt(int put(int c, void *cl), void *cl, char const *fmt, ...)
{
    RVaListBox_T box;
    va_start(box.ap, fmt);
    rfmt_vfmt(put, cl, fmt, &box);
    va_end(box.ap);
}

static int outc(int c, void *cl) { return putc(c, (FILE *)cl); }

void rfmt_print(char const *fmt, ...)
{
    RVaListBox_T box;
    va_start(box.ap, fmt);
    rfmt_vfmt(outc, stdout, fmt, &box);
    va_end(box.ap);
}

void rfmt_fprint(FILE *stream, char const *fmt, ...)
{
    RVaListBox_T box;
    va_start(box.ap, fmt);
    rfmt_vfmt(outc, stream, fmt, &box);
    va_end(box.ap);
}

int rfmt_sfmt(char *buf, int size, char const *fmt, ...)
{
    int          len;
    RVaListBox_T box;

    va_start(box.ap, fmt);
    len = rfmt_vsfmt(buf, size, fmt, &box);
    va_end(box.ap);
    return len;
}

static int insert(int c, void *cl)
{
    struct Buf_S *p = cl;
    if (p->bp >= p->buf + p->size) RAISE(RFmtOverflow);
    *p->bp++ = c;
    return c;
}

int rfmt_vsfmt(char *buf, int size, char const *fmt, RVaListBox_T *boxp)
{
    struct Buf_S cl;
    rassert(buf);
    rassert(size > 0);
    rassert(fmt);

    cl.buf = cl.bp = buf;
    cl.size        = size;

    rfmt_vfmt(insert, &cl, fmt, boxp);
    insert(0, &cl);

    return cl.bp - cl.buf - 1;
}

char *rfmt_string(char const *fmt, ...)
{
    char        *str;
    RVaListBox_T box;
    rassert(fmt);

    va_start(box.ap, fmt);
    str = rfmt_vstring(fmt, &box);
    va_end(box.ap);
    return str;
}

static int append(int c, void *cl)
{
    struct Buf_S *p = cl;
    if (p->bp >= p->buf + p->size) {
        RESIZE(p->buf, 2 * p->size);
        p->bp = p->buf + p->size;
        p->size *= 2;
    }
    *p->bp++ = c;
    return c;
}

char *rfmt_vstring(char const *fmt, RVaListBox_T *boxp)
{
    struct Buf_S cl;
    rassert(fmt);
    cl.size = 256;
    cl.buf = cl.bp = ALLOC(cl.size);
    rfmt_vfmt(append, &cl, fmt, boxp);
    append(0, &cl);
    return RESIZE(cl.buf, cl.bp - cl.buf);
}

/* #ifdef HAVE_VA_LIST_AS_ARRAY */
/* #define MAKE_POINTER_FROM_VA_LIST_ARG(arg) ((va_list *)(arg)) */
/* #else */
/* #define MAKE_POINTER_FROM_VA_LIST_ARG(arg) (&(arg)) */
/* #endif */

void rfmt_vfmt(int put(int c, void *cl), void *cl, char const *fmt, RVaListBox_T *boxp)
{
    rassert(put);
    rassert(fmt);

    while (*fmt) {
        if (*fmt != '%' || *++fmt == '%')
            put((unsigned char)*fmt++, cl);
        else {
            unsigned char c, flags[256];
            int           width = INT_MIN, precision = INT_MIN;

            memset(flags, '\0', sizeof(flags));

            if (FmtFlags) {
                unsigned char ch = *fmt;
                for (; ch && strchr(FmtFlags, ch); ch = *++fmt) {
                    rassert(flags[ch] < 255);
                    flags[ch]++;
                }
            }

            if (*fmt == '*' || isdigit(*fmt)) {
                int n;
                if (*fmt == '*') {
                    n = va_arg(boxp->ap, int);
                    rassert(n != INT_MIN);
                    fmt++;
                } else {
                    int d;
                    for (n = 0; isdigit(*fmt); fmt++) {
                        d = *fmt - '0';
                        rassert(n <= (INT_MAX - d) / 10);
                        n = 10 * n + d;
                    }
                }
                width = n;
            }

            if (*fmt == '.' && (*++fmt == '*' || isdigit(*fmt))) {
                int n;
                if (*fmt == '*') {
                    n = va_arg(boxp->ap, int);
                    rassert(n != INT_MAX);
                    fmt++;
                } else {
                    int d;
                    for (n = 0; isdigit(*fmt); fmt++) {
                        d = *fmt - '0';
                        rassert(n <= (INT_MAX - d) / 10);
                        n = 10 * n + d;
                    }
                }
                precision = n;
            }
            c = *fmt++;
            rassert(ConvertTable[c]);
            /* va_lsit pointer */
            /* va_list apcopy; */
            /* va_copy(apcopy, ap); */
            /* (*ConvertTable[c])(c, &apcopy, put, cl, flags, width, precision); */
            (*ConvertTable[c])(c, boxp, put, cl, flags, width, precision);
        }
    }
}

T rfmt_register(int code, T newcvt)
{
    T old;

    rassert(0 < code && code < (int)(sizeof(ConvertTable) / sizeof(ConvertTable[0])));
    old                = ConvertTable[code];
    ConvertTable[code] = newcvt;
    return old;
}

void rfmt_putd(char const *str, int len, int put(int c, void *cl), void *cl,
               unsigned char flags[256], int width, int precision)
{
    int sign;
    rassert(str);
    rassert(len >= 0);
    rassert(flags);

    if (width == INT_MIN) width = 0;
    if (width < 0) {
        flags['-'] = 1;
        width      = -width;
    }

    if (precision >= 0) flags['0'] = 0;
    if (len > 0 && (*str == '-' || *str == '+')) {
        sign = *str++;
        len--;
    } else if (flags['+']) {
        sign = '+';
    } else if (flags[' ']) {
        sign = ' ';
    } else {
        sign = 0;
    }

    {
        int n;
        if (precision < 0) precision = 1;

        if (len < precision)
            n = precision;
        else if (precision == 0 && len == 1 && str[0] == '0')
            n = 0;
        else
            n = len;

        if (sign) n++;
        if (flags['-']) {
            if (sign) put(sign, cl);
        } else if (flags['0']) {
            if (sign) put(sign, cl);
            PAD(width - n, '0');
        } else {
            PAD(width - n, ' ');
            if (sign) put(sign, cl);
        }
        PAD(precision - len, '0');
        {
            for (int i = 0; i < len; i++) put((unsigned char)*str++, cl);
        }
        if (flags['-']) PAD(width - n, ' ');
    }
}
