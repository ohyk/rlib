#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rassert.h"
#include "bufferio.h"
#include "rmem.h"

#define RMIN(x, y) ((x) < (y) ? (x) : (y))

typedef enum { BT_TYPE_ERR, BT_TYPE_INT, BT_TYPE_STR, BT_TYPE_LIST, BT_TYPE_DICT } Type_T;

typedef struct String_T
{
    unsigned length;
    char    *str;
} *String_T;

typedef struct Link_T *Link_T;
typedef struct Dict_T *Dict_T;

/* Functions Part */
/* Base Functions */
void print_space(int n, FILE *outfp);
/* int buf_getchar_fp(FILE *fp); */
/* void buf_ungetchar_fp(int c); */
/* void print_hex(const char *str, FILE *outfp); */
void print_hex(String_T s, FILE *outfp);
void print_string(String_T s, FILE *outfp);

/* Dictionary Functions */
void   dict_ele_destroy(void *dx);
void   dict_make_empty(void (*del)(void *), Dict_T l);
int    dict_is_empty(Dict_T l);
int    dict_size(Dict_T l);
Dict_T dict_create(void);
void   dict_insert(String_T key, void *x, Type_T t, Dict_T l);
void  *dict_find_key(const char *key, Dict_T l);
void  *dict_find_kth(int n, Dict_T l);
void   dict_delete_kth(void (*del)(void *), int n, Dict_T l);
void   dict_make_empty(void (*del)(void *), Dict_T l);
void   dict_print(Dict_T d, int depth);
/* Link Functions */
void   link_ele_destroy(void *lx);
void   link_make_empty(void (*del)(void *), Link_T l);
int    link_is_empty(Link_T l);
int    link_size(Link_T l);
Link_T link_create(void);
void   link_insert(void *x, Type_T t, Link_T l);
void  *link_find_kth(int n, Link_T l);
void   link_delete_kth(void (*del)(void *), int n, Link_T l);
void   link_make_empty(void (*del)(void *), Link_T l);
void   link_print(Link_T l, int depth);
/* Functions End */

/* Base Part */
/* static int Buf_size = -1; */
/* static char Buf_chars[128]; */
/* int buf_getchar_fp(FILE *fp) */
/* { */
/* if (Buf_size < 0) */
/* return fgetc(fp); */
/* else  */
/* return Buf_chars[Buf_size--]; */
/* } */

/* void buf_ungetchar_fp(int c) */
/* { */
/* if (Buf_size < 128)  */
/* Buf_chars[++Buf_size] = c; */
/* else  */
/* rassert(!"buffer of chars full"); */
/* } */

int check_type_health(int t)
{
    switch (t) {
        case BT_TYPE_INT:
        case BT_TYPE_STR:
        case BT_TYPE_LIST:
        case BT_TYPE_DICT:
            return 1;
    }

    return 0;
}

void print_space(int n, FILE *outfp)
{
    rassert(outfp);
    rassert(n >= 0);
    for (int i = 0; i < n; i++) fputc('\t', outfp);
}

void print_hex(String_T s, FILE *outfp)
{
    unsigned i;
    int      c;
    rassert(s && outfp);

    for (i = 0; i < s->length; i++) {
        c = s->str[i] & 0xff;
        fprintf(outfp, "%02x", c);
        if (((i + 1) & 1) == 0) fputc(' ', outfp);
    }
}

void print_string(String_T s, FILE *outfp)
{
    int i;
    rassert(s);

    for (i = 0; i < s->length; i++) fputc(s->str[i], outfp);
}

void string_destroy(void **sp)
{
    String_T str = *sp;
    FREE(str->str);
    FREE(*sp);
}

const char *type_to_str(Type_T t)
{
    switch (t) {
        case BT_TYPE_INT:
            return "INTEGER";
        case BT_TYPE_STR:
            return "STRING";
        case BT_TYPE_DICT:
            return "DICT";
        case BT_TYPE_LIST:
            return "LIST";
        case BT_TYPE_ERR:
            return "ERROR";
    }
    printf("?? %d\n", t);
    rassert(0);
}
/* Base End */

/* Dictionary Part */
struct Dict_T
{
    int size;
    struct dele
    {
        String_T     key;
        void        *x;
        Type_T       type;
        struct dele *next;
    } *link;
};

int dict_is_empty(Dict_T l)
{
    rassert(l);
    return (l->size == 0);
}

int dict_size(Dict_T l)
{
    rassert(l);
    return l->size;
}

Dict_T dict_create(void)
{
    Dict_T l = ALLOC(sizeof(*l));
    rassert(l);
    l->size = 0;
    l->link = NULL;
    return l;
}

void dict_insert(String_T key, void *x, Type_T t, Dict_T l)
{
    struct dele *e = ALLOC(sizeof(*e));
    rassert(key);
    rassert(check_type_health(t));
    rassert(l);

    e->x    = x;
    e->key  = key;
    e->type = t;
    e->next = l->link;
    l->link = e;
    l->size++;
}

void *dict_find_key(const char *key, Dict_T l)
{
    struct dele *e;
    int          klen;

    rassert(l);
    rassert(key);
    klen = strlen(key);

    for (e = l->link; e && strncmp(key, e->key->str, RMIN(klen, e->key->length)) != 0;
         e = e->next)
        continue;

    return e;
}

void *dict_find_kth(int n, Dict_T l)
{
    int          i;
    struct dele *e;

    rassert(l);
    rassert(n >= 0);

    for (i = 0, e = l->link; i < l->size && i < n; i++, e = e->next) continue;
    return (i == n) ? e : NULL;
}

void dict_ele_destroy(void *dx)
{
    struct dele *de = dx;

    string_destroy((void **)&de->key);
    switch (de->type) {
        case BT_TYPE_INT:
            break;
        case BT_TYPE_STR:
            string_destroy((void **)&de->x);
            break;
        case BT_TYPE_DICT:
            dict_make_empty(dict_ele_destroy, de->x);
            break;
        case BT_TYPE_LIST:
            link_make_empty(link_ele_destroy, de->x);
            break;
        case BT_TYPE_ERR:
        default:
            rassert(0);
    }
    if (de->x) FREE(de->x);
    FREE(de);
}

void dict_delete_kth(void (*del)(void *), int n, Dict_T l)
{
    struct dele *p;
    struct dele *t;
    int          i;

    rassert(l);
    rassert(n >= 0);
    for (i = 0, p = l->link; i < l->size && i < n; i++) {
        t = p;
        p = p->next;
    }
    if (i == n) {
        if (p == l->link)
            l->link = p->next;
        else
            t->next = p->next;
        if (del)
            del(p);
        else
            FREE(p);
        l->size--;
    }
}

void dict_make_empty(void (*del)(void *), Dict_T l)
{
    rassert(l);
    while (!dict_is_empty(l)) {
        dict_delete_kth(del, 0, l);
    }
}

void dict_ele_print(struct dele *e, int depth)
{
    rassert(e);
    print_space(depth, stdout);
    /* printf("- key: %s\n", e->key); */
    printf("- key: ");
    print_string(e->key, stdout);
    putchar('\n');
    print_space(depth, stdout);
    printf("- type: %s\n", type_to_str(e->type));
    print_space(depth, stdout);
    printf("- val: ");
    switch (e->type) {
        case BT_TYPE_INT:
            printf("%ld", *(long *)e->x);
            break;
        case BT_TYPE_STR:
            if (strncmp(e->key->str, "pieces", RMIN(e->key->length, strlen("pieces"))) ==
                0)
                print_hex(e->x, stdout);
            else
                print_string(e->x, stdout);
            /* printf("%s", (char *)e->x); */
            break;
        case BT_TYPE_DICT:
            printf("dict\n");
            dict_print((Dict_T)e->x, depth + 1);
            break;
        case BT_TYPE_LIST:
            printf("link\n");
            link_print((Link_T)e->x, depth + 1);
            break;
        case BT_TYPE_ERR:
            rassert(!"asdf");
            break;
    }
    putchar('\n');
}

void dict_print(Dict_T d, int depth)
{
    rassert(d);
    int i;

    if (dict_is_empty(d)) return;

    for (i = 0; i < d->size; i++) {
        dict_ele_print(dict_find_kth(i, d), depth);
    }
}

void dict_print_key(Dict_T d)
{
    rassert(d);
    int i;

    if (dict_is_empty(d)) return;

    for (i = 0; i < d->size; i++) {
        printf("KEY: ");
        print_string(((struct dele *)dict_find_kth(i, d))->key, stdout);
        putc('\n', stdout);
    }
}
/* Dictionary End */

/* Link Part */
struct Link_T
{
    int size;
    struct ele
    {
        void       *x;
        Type_T      type;
        struct ele *next;
    } *link;
};

int link_is_empty(Link_T l)
{
    rassert(l);
    return (l->size == 0);
}

int link_size(Link_T l)
{
    rassert(l);
    return l->size;
}

Link_T link_create(void)
{
    Link_T l = ALLOC(sizeof(*l));
    rassert(l);
    l->size = 0;
    l->link = NULL;
    return l;
}

void link_insert(void *x, Type_T t, Link_T l)
{
    struct ele *e = ALLOC(sizeof(*e));
    rassert(l);
    rassert(e);

    e->x    = x;
    e->type = t;
    e->next = l->link;
    l->link = e;
    l->size++;
}

void *link_find_kth(int n, Link_T l)
{
    int         i;
    struct ele *e;

    rassert(l);
    rassert(n >= 0);

    for (i = 0, e = l->link; i < l->size && i < n; i++, e = e->next) continue;
    return (i == n) ? e : NULL;
}

void link_ele_destroy(void *lx)
{
    struct ele *le = lx;

    switch (le->type) {
        case BT_TYPE_INT:
            break;
        case BT_TYPE_STR:
            string_destroy((void **)&le->x);
            break;
        case BT_TYPE_DICT:
            dict_make_empty(dict_ele_destroy, le->x);
            break;
        case BT_TYPE_LIST:
            link_make_empty(link_ele_destroy, le->x);
            break;
        case BT_TYPE_ERR:
        default:
            rassert(0);
    }
    if (le->x) FREE(le->x);
    FREE(le);
}

void link_delete_kth(void (*del)(void *), int n, Link_T l)
{
    struct ele *p;
    struct ele *t;
    int         i;

    rassert(l);
    rassert(n >= 0);
    for (i = 0, p = l->link; i < l->size && i < n; i++) {
        t = p;
        p = p->next;
    }
    if (i == n) {
        if (p == l->link)
            l->link = p->next;
        else
            t->next = p->next;
        if (del)
            del(p);
        else
            FREE(p);
        l->size--;
    }
}

void link_make_empty(void (*del)(void *), Link_T l)
{
    rassert(l);
    while (!link_is_empty(l)) {
        link_delete_kth(del, 0, l);
    }
}

void link_ele_print(struct ele *e, int depth)
{
    rassert(e);
    print_space(depth, stdout);
    printf("- type: %s\n", type_to_str(e->type));
    print_space(depth, stdout);
    printf("- val: ");
    switch (e->type) {
        case BT_TYPE_INT:
            printf("%ld", *(long *)e->x);
            break;
        case BT_TYPE_STR:
            print_string(e->x, stdout);
            /* printf("%s", (char *)e->x); */
            break;
        case BT_TYPE_DICT:
            printf("dict\n");
            dict_print((Dict_T)e->x, depth + 1);
            break;
        case BT_TYPE_LIST:
            printf("link\n");
            link_print((Link_T)e->x, depth + 1);
            break;
        case BT_TYPE_ERR:
            rassert(!"asdf");
            break;
    }
    putchar('\n');
}

void link_print(Link_T l, int depth)
{
    rassert(l);
    int i;

    if (link_is_empty(l)) return;

    for (i = 0; i < l->size; i++) {
        link_ele_print(link_find_kth(i, l), depth);
    }
}
/* Link End */

/* Parse Part */
long *get_integer(const char *str, char **endptr)
{
    long  num  = 0;
    long *np   = NULL;
    char *endp = NULL;
    rassert(str);
    rassert(isdigit(*str) || *str == '-');

    errno = 0;
    if ((num = strtol(str, &endp, 10)) == 0 && errno == EINVAL)
        rassert(!"get integer fault");
    if (*endp != 'e') rassert(!"get integer fault");

    *endptr = endp + 1;
    np      = ALLOC(sizeof(*np));
    *np     = num;
    return np;
}

long *get_integer_fp(FILE *fp)
{
    int   c, i;
    long  num  = 0;
    long *np   = NULL;
    char *endp = NULL;
    char  buf[43];

    rassert(fp);

    if ((c = buf_getchar_fp(fp)) != '-' && !isdigit(c)) rassert(!"get integer fault");

    i        = 0;
    buf[i++] = c;
    while (isdigit((c = buf_getchar_fp(fp)))) {
        buf[i++] = c;
    }
    buf[i] = '\0';
    if (c != 'e') rassert(!"get integer fault");

    errno = 0;
    if ((num = strtol(buf, &endp, 10)) == 0 && errno == EINVAL)
        rassert(!"get integer conver fault");
    if (errno == ERANGE) rassert(!"get integer range fault");
    if (*endp != '\0') rassert(!"get integer conver fault");

    np  = ALLOC(sizeof(*np));
    *np = num;
    return np;
}

String_T get_string_fp(FILE *fp)
{
    int      c;
    int      i;
    long     sn = -1;
    char     buf[43];
    char    *dstr = NULL;
    char    *endp = NULL;
    String_T ds   = NULL;

    rassert(fp);

    i = 0;
    while (isdigit((c = buf_getchar_fp(fp)))) {
        buf[i++] = c;
    }
    buf[i] = '\0';
    if (c != ':') rassert(!"get string fault");

    errno = 0;
    if ((sn = strtol(buf, &endp, 10)) == 0 && errno == EINVAL) {
        perror(__func__);
        fprintf(stderr, "%s\n", buf);
        rassert(!"get string argument inval fault");
    }
    if (errno == ERANGE) rassert(!"get string range fault");
    if (*endp != '\0') rassert(!"get string length conver fault");

    if (sn < 0) rassert(!"get string range negative");

    dstr = ALLOC(sn /*+1*/);
    if (buf_getblock_fp(sn, dstr, fp) != sn) rassert(!"get string fault");

    /* for (i = 0; i < sn; i++) */
    /* dstr[i] = buf_getchar_fp(fp); */
    /* if (i != sn) */
    /* rassert(!"get string length fault"); */

    ds         = ALLOC(sizeof(*ds));
    ds->length = sn;
    ds->str    = dstr;
    return ds;
}

char *get_string(const char *str, char **endptr)
{
    char *dstr = NULL;
    int   sn   = 0;
    char *endp = NULL;

    rassert(str);
    rassert(isdigit(*str));

    errno = 0;
    if ((sn = strtol(str, &endp, 10)) == 0 && errno == EINVAL)
        rassert(!"get string length fault");
    if (*endp != ':') rassert(!"get string length fault");

    dstr = ALLOC(sn + 1);
    memcpy(dstr, endp + 1, sn);
    dstr[sn] = '\0';

    *endptr = endp + sn + 1;
    return dstr;
}

Type_T match_type(const char *str, char **endptr, void **val);
Type_T match_type_fp(FILE *fp, void **val);

Link_T get_link_fp(FILE *fp)
{
    Link_T l = link_create();
    Type_T t = BT_TYPE_ERR;
    void  *x = NULL;
    int    c;

    rassert(fp);

    while ((t = match_type_fp(fp, &x)) != BT_TYPE_ERR) {
        link_insert(x, t, l);
        if ((c = buf_getchar_fp(fp)) == 'e')
            break;
        else
            buf_ungetchar_fp(c);
    }

    return l;
}

Link_T get_link(const char *str, char **endptr)
{
    Link_T l    = link_create();
    Type_T t    = BT_TYPE_ERR;
    char  *strp = (char *)str;
    char  *endp = NULL;
    void  *x    = NULL;
    rassert(str);
    rassert(endptr);

    while ((t = match_type(strp, &endp, &x)) != BT_TYPE_ERR) {
        link_insert(x, t, l);
        if (*endp == 'e') break;
        strp = endp;
    }

    *endptr = endp + 1;
    return l;
}

Dict_T get_dict_fp(FILE *fp)
{
    Dict_T   d   = dict_create();
    void    *x   = NULL;
    String_T key = NULL;
    Type_T   t   = BT_TYPE_ERR;
    int      c;

    rassert(fp);

    while ((t = match_type_fp(fp, &x)) == BT_TYPE_STR) {
        key = x;
        if ((t = match_type_fp(fp, &x)) == BT_TYPE_ERR) rassert(!"get dict val fault");
        dict_insert(key, x, t, d);
        if ((c = buf_getchar_fp(fp)) == 'e')
            break;
        else
            buf_ungetchar_fp(c);
    }

    return d;
}

Dict_T get_dict(const char *str, char **endptr)
{
    Dict_T   d    = dict_create();
    char    *endp = NULL;
    char    *strp = (char *)str;
    void    *x    = NULL;
    String_T key  = NULL;
    Type_T   t    = BT_TYPE_ERR;
    rassert(str);
    rassert(endptr);

    while ((t = match_type(strp, &endp, &x)) == BT_TYPE_STR) {
        key = x;
        if ((t = match_type(endp, &strp, &x)) != BT_TYPE_ERR) {
            dict_insert(key, x, t, d);
        }
        if (*strp == 'e') {
            break;
        }
    }

    *endptr = strp + 1;
    return d;
}

Type_T match_type_fp(FILE *fp, void **val)
{
    Type_T t = BT_TYPE_ERR;
    int    c;
    void  *v = NULL;

    rassert(fp);
    rassert(val);

    c = buf_getchar_fp(fp); /* get one char */
    if (isdigit(c)) {       /* string */
        t = BT_TYPE_STR;
        buf_ungetchar_fp(c);
        v = get_string_fp(fp);
    } else if (c == 'i') { /* integer */
        t = BT_TYPE_INT;
        v = get_integer_fp(fp);
    } else if (c == 'd') { /* dictionary */
        t = BT_TYPE_DICT;
        v = get_dict_fp(fp);
    } else if (c == 'l') { /* list */
        t = BT_TYPE_LIST;
        v = get_link_fp(fp);
    } else {
        fprintf(stderr, "== %c\n", c);
        rassert(!"match type fault <unknown type>");
    }
    *val = v;
    return t;
}

Type_T match_type(const char *str, char **endptr, void **val)
{
    Type_T t    = BT_TYPE_ERR;
    char  *endp = NULL;
    void  *v    = NULL;
    rassert(str);
    rassert(endptr);

    if (isdigit(*str)) { /* string */
        t = BT_TYPE_STR;
        v = get_string(str, &endp);
    } else if (*str == 'i') { /* integer */
        t = BT_TYPE_INT;
        v = get_integer(str + 1, &endp);
    } else if (*str == 'l') { /* list */
        t = BT_TYPE_LIST;
        v = get_link(str + 1, &endp);
    } else if (*str == 'd') { /* dict */
        t = BT_TYPE_DICT;
        v = get_dict(str + 1, &endp);
    } else { /* unknow */
        printf("== %s\n", str);
        rassert(!"match type fault <unknown type>");
    }

    *endptr = endp;
    *val    = v;
    return t;
}
/* Parse End */

/* Bt_T parse_bt(FILE *bt_fp) */
/* { */
/* } */

/* int *make_int(int i) */
/* { */
/* int *np = ALLOC(sizeof(int)); */
/* *np = i; */
/* return np; */
/* } */

/* static const char *strs[] = { */
/* "zero", */
/* "one", */
/* "two", */
/* "three", */
/* "four", */
/* "five", */
/* "six", */
/* "seven", */
/* "eight", */
/* "ten" */
/* }; */

char *get_file_data(FILE *fp, unsigned long *size)
{
    char         *f  = NULL;
    unsigned long gn = 0;
    unsigned long n  = 0;
    char          buf[BUFSIZ];
    rassert(fp);
    rassert(size);

    while (!feof(fp)) {
        gn = fread(buf, 1, BUFSIZ, fp);
        if (!f) {
            f = ALLOC(gn);
            memcpy(f, buf, gn);
        } else {
            RESIZE(f, gn + n);
            memcpy(f + n, buf, gn);
        }
        /* printf("%ld\n", gn); */
        n += gn;
    }
    *size = n;
    return f;
}

Dict_T parse_file(FILE *fp)
{
    Dict_T d = NULL;

    rassert(fp);
    rassert(buf_getchar_fp(fp) == 'd');
    d = get_dict_fp(fp);
    return d;
}

int main(int argc, char *argv[])
{
    /* char *str   = NULL; */
    /* char *endp  = NULL; */
    /* char buf[BUFSIZ]; */
    FILE *fp = NULL;

    /* while (fgets(buf, BUFSIZ, stdin) != NULL) { */
    /* str = get_string(buf, &endp); */
    /* printf(" --- get string: [%s]\n", str); */
    /* printf("endp: [%s]\n", endp); */
    /* FREE(str); */

    /* if (*buf == 'i') */
    /* printf("%d\n", *get_integer(buf+1, &endp)); */
    /* } */

    /* int i; */
    /* Dict_T d = dict_create(); */
    /* for (i = 0; i < 5; i++) { */
    /* dict_insert(strs[i], make_int(i), BT_TYPE_INT, d); */
    /* } */

    /* dict_print(d); */

    /* printf("find %s\n", strs[0]); */
    /* dict_ele_print(dict_find_key(strs[0], d)); */
    /* printf("find %s\n", strs[5]); */
    /* if (dict_find_key(strs[5], d)) */
    /* dict_ele_print(dict_find_key(strs[5], d)); */

    Dict_T d = NULL;

    if (argc != 2) return 2;

    if ((fp = fopen(argv[1], "r")) == NULL) {
        perror(argv[0]);
        return 1;
    }

    /* unsigned long size = 0; */
    /* char *file = get_file_data(fp, &size); */
    /* rassert(*file == 'd'); */
    /* d = get_dict(file+1, &endp); */

    /* fprintf(stderr, "file size: %ld\n", size); */
    /* FREE(file); */

    /* while (fgets(buf, BUFSIZ, stdin) != NULL) { */
    /* d = get_dict(buf+1, &endp); */
    /* } */

    /* fp = stdin; */

    /* rassert(buf_getchar_fp(fp) == 'd'); */
    /* d = get_dict_fp(fp); */
    d = parse_file(fp);

    /* dict_print_key(d); */
    dict_print(d, 0);

    /* struct dele *de = NULL; */
    /* while (fgets(buf, BUFSIZ, stdin) != NULL) { */
    /* de = dict_find_key(buf, d); */
    /* if (de) */
    /* dict_ele_print(de, 0); */
    /* } */

    /* struct dele *de = NULL; */
    /* de = dict_find_key("info", d); */
    /* de = dict_find_key("pieces", de->x); */
    /* if (de) { */
    /* print_hex(de->x, stdout); */
    /* putchar('\n'); */
    /* } */

#ifdef MEM_CHK
    print_allocated();
    print_freed();
#endif

    /* getchar(); */

    /* dict_make_empty(dict_ele_destroy, d); */

#ifdef MEM_CHK
    print_allocated();
    print_freed();
#endif
    /* getchar(); */

    return 0;
}
