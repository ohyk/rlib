#include "rtools.h"

#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <io.h>
#else
#include <unistd.h>
#endif

#include "rassert.h"
#include "rexcept.h"
#include "rmem.h"
#include "rrandom.h"
#include "rvector.h"

/*====================================================================*/
/*
 * Macros Part
 */
/*====================================================================*/
#ifndef LNS_BUF
#define LNS_BUF 4096
#endif /* ifndef LNS_BUF */

#ifndef GET_PTR_OFFSET
#define GET_PTR_OFFSET(a, i, s) ((a) + ((i) * (s)))
#endif /* ifndef GET_PTR_OFFSET */

#ifndef ELEM_BUF_LEN
#define ELEM_BUF_LEN 128
#endif /* ifndef ELEM_BUF_LEN */

/*====================================================================*/
/*
 * Exception Part
 */
/*====================================================================*/
RExcept_T const RToolsRangeOverflow = {"String to integer range overflow"};

/*====================================================================*/
/*
 * Static Functions Part
 */
/*====================================================================*/
static long str_to_l(char const *str, char **endp, int base)
{
    rassert(str);
    long  res;
    char *ep;

    if (!endp) endp = &ep;
    errno = 0;
    if (((res = strtol(str, endp, base)) == 0 && errno == EINVAL) || *endp == str) {
        fprintf(stderr, "String to long failed!\nstring: [%s]\n", str);
        perror("errno");
        rassert(0);
    }
    if (errno == ERANGE) RAISE(RToolsRangeOverflow);
    return res;
}

/*====================================================================*/
/*
 * Functions Part
 */
/*====================================================================*/
long rtools_string_to_long(char const *str, int base) { return str_to_l(str, NULL, 10); }

long rtools_string_to_long_endp(char const *str, char **endp, int base)
{
    return str_to_l(str, endp, base);
}

int *rtools_get_line_numbers_fd(int fd)
{
    off_t cur_oft;
    char  buf[LNS_BUF];
    int   i, k;
    /* int line_count; */
    int       ch_count;
    int       read_count;
    RVector_T lines = rvector_new(sizeof(int), NULL);

    rassert(fd >= 0);

    /* first keep fd current offset then set offset to start */
    errno = 0;
    if ((cur_oft = lseek(fd, 0, SEEK_CUR)) == -1 || lseek(fd, 0, SEEK_SET) == -1) {
        perror(__func__);
        rassert(0);
    }

    /* first line */
    k = 0;
    rvector_append(lines, &k);

    for (ch_count = 0; (read_count = read(fd, buf, LNS_BUF)) > 0;
         ch_count += read_count) {
        for (i = 0; i < read_count; i++) {
            if (buf[i] == '\n') {
                k = ch_count + i + 1;
                rvector_append(lines, &k);
                /* line_count++; */
            }
        }
    }

    /* descard last */
    rvector_subtract(lines);

    /* add terminator -1 */
    k = -1;
    rvector_append(lines, &k);

    /* restore current offset */
    if (lseek(fd, cur_oft, SEEK_SET) == -1) {
        perror(__func__);
        rassert(0);
    }

    return rvector_get_dispose(&lines);
}

char *rtools_string_copy(char const *src, int len)
{
    char *dsc = NULL;

    rassert(src && len > 0);

    return dsc;
}

int rtools_get_number_of_digit(int n, int base)
{
    unsigned u;
    int      i;

    rassert(base > 0);

    if (n == INT_MIN)
        u = INT_MAX + 1U;
    else if (n < 0)
        u = -n;
    else
        u = n;

    for (i = 0; u > 0; u /= base) i++;
    return i;
}

static char *string_copy(char const *str, int len)
{
    char *res = RTOOLS_ALLOC(len + 1);
    int   i;
    for (i = 0; i < len && str[i]; i++) res[i] = str[i];
    res[i] = '\0';
    return res;
}

char **rtools_split_string_with_delimiter(char const *str, int len, int delm)
{
    char     *s = NULL;
    char      c = '\0';
    int       i;
    RVector_T sbuf = rvector_new(sizeof(s), NULL);
    RVector_T cbuf = rvector_new(sizeof(c), NULL);

    for (i = 0; str[i]; ++i) {
        if (str[i] == delm) {
            // rvector_append(cbuf, &c); // '\0' terminator
            s = string_copy(rvector_get(cbuf), rvector_length(cbuf));
            rvector_append(sbuf, &s);
            rvector_reset(cbuf);  // reset char buf
        } else
            rvector_append(cbuf, (void *)&str[i]);
    }

    if (rvector_length(cbuf) > 0) {
        /* rvector_append(cbuf, &c); */
        s = string_copy(rvector_get(cbuf), rvector_length(cbuf));
        rvector_append(sbuf, &s);
    }

    // string array terminator
    s = NULL;
    rvector_append(sbuf, &s);

    // clean
    rvector_get_dispose(&cbuf);

    return rvector_get_dispose(&sbuf);
}

char *rtools_squeeze_repeated_string_one(char *str, int len, int (*isc)(int))
{
    rassert(str);
    rassert(len > 0);
    rassert(isc);
    char *s, *q;
    int   i;
    for (s = str, q = str + 1, i = 0; i < len && *q; i++, q++) {
        if (*s != *q) {
            if (isc(*s) && q - s > 1) {  // repeated
                memcpy(s + 1, q, len - i);
                q = s + 1;
            }
            s++;
        }
    }
    if (isc(*s) && q - s > 1) s[1] = '\0';
    return str;
}

void *rword_binary_search(void *array, size_t elem_size, size_t elem_count,
                          void const *key, int (*cmp)(void const *, void const *))
{
    rassert(array);
    rassert(cmp);
    rassert(elem_size > 0);
    rassert(elem_count > 0);

    int s, e;
    int mid;

    s = 0;
    e = elem_count - 1;

    while (s <= e) {
        mid = s + (e - s) / 2;
        if (cmp(GET_PTR_OFFSET(array, mid, elem_size), &key) < 0)
            s = mid + 1;
        else if (cmp(GET_PTR_OFFSET(array, mid, elem_size), &key) > 0)
            e = mid - 1;
        else
            return GET_PTR_OFFSET(array, mid, elem_size);
    }
    return NULL;
}

void rtools_shell_sort(void *array, size_t elem_size, size_t elem_count,
                       int (*cmp)(void const *, void const *))
{
    rassert(array);
    rassert(cmp);
    rassert(elem_count > 0);
    rassert(elem_size > 0);
    rassert(elem_size < ELEM_BUF_LEN);

    int  i, j, h;
    int  l, r;
    char buf[ELEM_BUF_LEN];

    l = 0;
    r = elem_count - 1;

    for (h = 1; h <= (r - l) / 9; h = 3 * h + 1) continue;
    for (; h > 0; h /= 3)
        for (i = l + h; i <= r; i++) {
            j = i;
            memcpy(buf, GET_PTR_OFFSET(array, j, elem_size), elem_size);
            while (j >= l + h && cmp(buf, GET_PTR_OFFSET(array, j - h, elem_size)) < 0) {
                memcpy(GET_PTR_OFFSET(array, j, elem_size),
                       GET_PTR_OFFSET(array, j - h, elem_size), elem_size);
                j -= h;
            }
            memcpy(GET_PTR_OFFSET(array, j, elem_size), buf, elem_size);
        }
}

void rtools_disrupt_array(void *array, size_t elem_size, size_t elem_count)
{
    rassert(array);
    rassert(elem_size > 0);
    rassert(elem_count > 0);
    rassert(elem_size < ELEM_BUF_LEN);

    char buf[ELEM_BUF_LEN];

    for (int i = 1; i < elem_count; ++i) {
        int m = rrandom_uniform_int(0, i);
        { /* swap */
            memcpy(buf, GET_PTR_OFFSET(array, i, elem_size), elem_size);
            memcpy(GET_PTR_OFFSET(array, i, elem_size),
                   GET_PTR_OFFSET(array, m, elem_size), elem_size);
            memcpy(GET_PTR_OFFSET(array, m, elem_size), buf, elem_size);
        }
    }
}

char *rtools_string_center(char const *str, int len, int padc)
{
    rassert(str);
    rassert(len > 0);

    int   slen = strlen(str);
    char *res  = NULL;

    if (slen > len) {
        res = string_copy(str, slen);
    } else {
        res             = RTOOLS_ALLOC(len + 1);
        int nspace      = len - slen;
        int half_nspace = nspace / 2;
        int i, j;
        /* left space */
        for (i = 0; i < half_nspace; ++i) res[i] = padc;
        /* string */
        for (j = 0; j < slen; ++i, ++j) res[i] = str[j];
        /* right space */
        for (j = 0; j < nspace - half_nspace; ++i, ++j) res[i] = padc;
        res[i] = '\0';
    }

    return res;
}

/*================================ Match =================================*/
/*
 *
 */
/*========================================================================*/

static int match_star(int c, char const *regexp, char const *text);
static int match_here(char const *regexp, char const *text)
{
    if (regexp[0] == '\0') return 1;
    if (regexp[1] == '*') return match_star(regexp[0], regexp + 2, text);
    if (regexp[0] == '$' && regexp[1] == '\0') return *text == '\0';
    if (*text != '\0' && (regexp[0] == '.' || regexp[0] == *text))
        return match_here(regexp + 1, text + 1);
    return 0;
}

static int match_star(int c, char const *regexp, char const *text)
{
    do {
        if (match_here(regexp, text)) return 1;
    } while (*text != '\0' && (*text++ == c || c == '.'));
    return 0;
}

int rtools_match(char const *regexp, char const *text)
{
    rassert(regexp);
    rassert(text);

    if (regexp[0] == '^') return match_here(regexp + 1, text);
    do {
        if (match_here(regexp, text)) return 1;
    } while (*text++ != '\0');

    return 0;
}

int *rtools_kmp_make_jump(char const *dstr, int len)
{
    int       k;
    int      *res;
    RVector_T buf = rvector_new(sizeof(k), NULL);
    k             = 0;
    rvector_append(buf, &k); /* res[0] = 0 */
    rvector_append(buf, &k); /* res[1] = 0, set bound */

    for (int i = 1; i < len; ++i) {
        res   = rvector_get(buf);
        int j = res[i];
        while (j && dstr[i] != dstr[j]) j = res[j];
        k = dstr[i] == dstr[j] ? j + 1 : 0;
        rvector_append(buf, &k); /* res[i+1] */
    }

    return rvector_get_dispose(&buf);
}

int rtools_kmp_find_string(char const *str, char const *dstr, int const *jump)
{
    rassert(str);
    rassert(dstr);
    int n   = strlen(str);
    int m   = strlen(dstr);
    int j   = 0; /* dstr pos */
    int res = -1;
    if (m > n) return -1;

    for (int i = 0; i < n; ++i) {
        while (j && str[i] != dstr[j]) j = jump[j];
        if (str[i] == dstr[j]) ++j; /* if equal, dstr move a step */
        if (j == m) {
            res = i - m + 1;
            break;
        }
    }

    return res;
}
