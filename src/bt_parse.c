#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bufferio.h"
#include "rassert.h"
#include "ratom.h"
#include "rmem.h"
#include "rseq.h"
#include "rtable.h"

#define BT_NEW(type, val, length) bt_new((type), (val), (length), __FILE__, __LINE__)
#define RMIN(x, y) ((x) < (y) ? (x) : (y))
#define OUTSPACE '\t'

#define List_T RSeq_T
#define Dict_T RTable_T

typedef enum {
    BT_TYPE_ERR,
    BT_TYPE_INT,
    BT_TYPE_STR,
    BT_TYPE_LIST,
    BT_TYPE_DICT
} Bt_Type_T;

typedef struct Bt_T
{
    void     *val;
    long      length;
    Bt_Type_T type;
} *Bt_T;

Bt_T bt_parse_value(FILE *fp);

static long string_to_long(char const *str)
{
    char *endp;
    long  n;

    errno = 0;
    if ((n = strtol(str, &endp, 10)) == 0 && errno == EINVAL)
        rassert(!"string to long invalid string");
    if (errno == ERANGE) rassert(!"string to long range fault");
    if (endp == str) rassert(!"string to long invalid string");
    return n;
}

static Bt_T bt_new(Bt_Type_T type, void *val, long length, char const *file, int line)
{
    Bt_T bt    = rmem_alloc(sizeof(*bt), file, line);
    bt->val    = val;
    bt->type   = type;
    bt->length = length;
    return bt;
}

Bt_T bt_get_integer_fp(FILE *fp)
{
    int   c, i;
    long  num = 0;
    long *np  = NULL;
    char  buf[64];

    rassert(fp);

    if ((c = buf_getchar()) != '-' && !isdigit(c)) rassert(!"get integer fault");

    i        = 0;
    buf[i++] = c;
    while (i < 64 && isdigit((c = buf_getchar()))) {
        buf[i++] = c;
    }
    buf[i] = '\0';
    if (c != 'e') rassert(!"get integer fault");

    num = string_to_long(buf);

    np  = ALLOC(sizeof(*np));
    *np = num;
    return BT_NEW(BT_TYPE_INT, np, 0);
}

Bt_T bt_get_string_fp(FILE *fp)
{
    int   c;
    int   i;
    long  sn = -1;
    char  buf[64];
    char *dstr = NULL;

    rassert(fp);

    i = 0;
    while (i < 64 && isdigit((c = buf_getchar()))) {
        buf[i++] = c;
    }
    buf[i] = '\0';
    if (c != ':') rassert(!"get string fault");

    sn = string_to_long(buf);
    if (sn < 0) rassert(!"get string range negative");

    dstr = ALLOC(sn);
    if (buf_getblock(sn, dstr) != sn) rassert(!"get string fault");

    return BT_NEW(BT_TYPE_STR, dstr, sn);
}

Bt_T bt_get_list_fp(FILE *fp)
{
    List_T l = rseq_new(0);
    Bt_T   x = NULL;
    int    c;

    rassert(fp);

    while ((x = bt_parse_value(fp)) != NULL) {
        rseq_addhi(l, x);
        if ((c = buf_getchar()) == 'e')
            break;
        else
            buf_ungetchar(c);
    }

    return BT_NEW(BT_TYPE_LIST, l, rseq_length(l));
}

/* atom key */
static int cmp_key(void const *x, void const *y)
{
    return memcmp(((Bt_T)x)->val, ((Bt_T)y)->val,
                  RMIN(((Bt_T)x)->length, ((Bt_T)y)->length) + 1);
}

/* static int cmp_js_key(void const *x, void const *y) */
/* { */
/* return ((Json_T) x)->val != ((Json_T) y)->val; */
/* } */

static unsigned hash_key(void const *x) { return (unsigned long)((Bt_T)x)->val >> 2; }

Bt_T bt_get_dict_fp(FILE *fp)
{
    Dict_T d   = rtable_new(0, cmp_key, hash_key);
    Bt_T   x   = NULL;
    void  *key = NULL;
    int    c;

    rassert(fp);

    while ((x = bt_parse_value(fp)) && x->type == BT_TYPE_STR) {
        key = (void *)ratom_new(x->val, x->length);
        FREE(x->val);
        x->val = key;
        key    = x;

        if ((x = bt_parse_value(fp)) && x->type == BT_TYPE_ERR)
            rassert(!"get dict fault");
        rtable_put(d, key, x);
        if ((c = buf_getchar()) == 'e')
            break;
        else
            buf_ungetchar(c);
    }

    return BT_NEW(BT_TYPE_DICT, d, rtable_length(d));
}

Bt_T bt_parse_value(FILE *fp)
{
    int c;

    rassert(fp);

    c = buf_getchar(); /* get one char */
    switch (c) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            buf_ungetchar(c);
            return bt_get_string_fp(fp);
        case 'i':
            return bt_get_integer_fp(fp);
        case 'd':
            return bt_get_dict_fp(fp);
        case 'l':
            return bt_get_list_fp(fp);
        default:
            return NULL;
    }
}

void bt_print_value(Bt_T bt, int depth);

static void print_space(int length, int space)
{
    if (length == 0) return;
    buf_fill_char(length, space);
}

static void print_hex(Bt_T str)
{
    int i;
    for (i = 0; i < str->length; i++) {
        buf_printf(3, "%02x", ((char *)str->val)[i]);
        if (((i + 1) & 1) == 0) buf_putchar(' ');
    }
    buf_putchar('\n');
}

static int cmp_str(void const *x, void const *y)
{
    if ((*(Bt_T *)x)->length > (*(Bt_T *)y)->length)
        return 1;
    else if ((*(Bt_T *)x)->length < (*(Bt_T *)y)->length)
        return -1;
    else
        return memcmp((*(Bt_T *)x)->val, (*(Bt_T *)y)->val, (*(Bt_T *)x)->length);
}

static void print_dict(Bt_T dict, int depth)
{
    Bt_T *ar = (Bt_T *)rtable_to_array(dict->val, NULL);
    qsort(ar, dict->length, 2 * sizeof(*ar), cmp_str);
    int i;
    for (i = 0; ar[i]; i += 2) {
        print_space(depth, OUTSPACE);
        buf_printf(9, " - KEY: ");
        buf_putblock(ar[i]->length, ar[i]->val);
        buf_putchar('\n');
        print_space(depth, OUTSPACE);
        buf_printf(9, " - VAL: ");
        if (6 == ar[i]->length && memcmp(ar[i]->val, "pieces", 6) == 0)
            print_hex(ar[i + 1]);
        else if (4 == ar[i]->length && memcmp(ar[i]->val, "ed2k", 4) == 0)
            print_hex(ar[i + 1]);
        else if (8 == ar[i]->length && memcmp(ar[i]->val, "filehash", 8) == 0)
            print_hex(ar[i + 1]);
        else
            bt_print_value(ar[i + 1], depth);
    }
}

static void print_list(Bt_T list, int depth)
{
    int i;
    for (i = 0; i < list->length; i++) {
        print_space(depth, OUTSPACE);
        buf_printf(9, " - VAL: ");
        bt_print_value(rseq_get(list->val, i), depth);
    }
}

void bt_print_value(Bt_T bt, int depth)
{
    rassert(bt);
    switch (bt->type) {
        case BT_TYPE_INT:
            buf_printf(64, "%ld", *(long *)(bt->val));
            break;
        case BT_TYPE_STR:
            buf_putblock(bt->length, bt->val);
            break;
        case BT_TYPE_DICT:
            buf_printf(64, "DICT (%d)\n", bt->length);
            print_dict(bt, depth + 1);
            break;
        case BT_TYPE_LIST:
            buf_printf(64, "LIST (%d)\n", bt->length);
            print_list(bt, depth + 1);
            break;
        case BT_TYPE_ERR:
        default:
            fprintf(stderr, "-- ??? %d\n", bt->type);
            rassert(0);
    }
    buf_putchar('\n');
}

char *bt_print(Bt_T bt)
{
    bt_print_value(bt, 0);
    buf_putchar('\0');
    return Global_Output_Buffer.buf;
}

Bt_T parse_file(FILE *fp)
{
    Bt_T d;

    rassert(fp);
    rassert(buf_getchar() == 'd');

    if ((d = bt_get_dict_fp(fp)) == NULL) rassert(0);
    return d;
}

void print_key(void const *key, void **x, void *cl)
{
    printf("Key: ");
    fwrite(((Bt_T)key)->val, 1, ((Bt_T)key)->length, stdout);
    putchar('\n');
}

int main(int argc, char *argv[])
{
    FILE *fp = NULL;
    Bt_T  d  = NULL;

    if (argc != 2) return 2;

    if ((fp = fopen(argv[1], "r")) == NULL) {
        perror(argv[0]);
        return 1;
    }

    Global_Input_Buffer.in = fp;

    if ((d = parse_file(fp)) == NULL) {
        rassert(0);
    }

    printf("%ld\n", ((Bt_T)d)->length);
    /* rtable_map(((Bt_T) d)->val, print_key, NULL); */
    char *out = bt_print(d);
    printf("%s\n", out);

    fprintf(stderr, "-- buffer --\n");
    fprintf(stderr, "buffer: %p\n", Global_Output_Buffer.buf);
    fprintf(stderr, "length: %ld\n", Global_Output_Buffer.length);
    fprintf(stderr, "length: %ld\n", Global_Output_Buffer.offset);
    fprintf(stderr, "-- buffer end --\n");

#ifdef MEM_CHK
    rmem_print_allocated();
#endif

    return 0;
}
