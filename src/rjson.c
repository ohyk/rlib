/*
 * ;; *.json -> Json_T
 * ;; parse json file
 */
#include "rjson.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "bufferio.h"
#include "ratom.h"
#include "rmem.h"
#include "rseq.h"
#include "rtable.h"

#define JSON_NEW(type, val) json_new((type), (val), __FILE__, __LINE__)

#define Object_T RTable_T
#define Array_T RSeq_T

#define ENTRY_DELIMITER ','
#define KEY_VAL_DELIMITER ':'
#define INPUT_BUFFER_CHAR_MAX 128
#define DIGIT_STRS_LENGTH_MAX 64

#ifndef BUFSIZ
#define BUFSIZ 4096
#endif

static Json_T json_parse_value(void);

static char *False_Str = "false";
static char *True_Str  = "true";
static char *Null_Str  = "null";
static char  Internal_Buffer[BUFSIZ];

static void print_str_unescape_seq(Output_Buffer_T *obp, char const *str)
{
    while (*str) {
        switch (*str) {
            case '/':
                buf_putstr_outbuffer(obp, "/");
                break;
            case '\\':
                buf_putstr_outbuffer(obp, "\\\\");
                break;
            case '"':
                buf_putstr_outbuffer(obp, "\\\"");
                break;
            case '\b':
                buf_putstr_outbuffer(obp, "\\b");
                break;
            case '\n':
                buf_putstr_outbuffer(obp, "\\n");
                break;
            case '\t':
                buf_putstr_outbuffer(obp, "\\t");
                break;
            case '\r':
                buf_putstr_outbuffer(obp, "\\r");
                break;
            case '\f':
                buf_putstr_outbuffer(obp, "\\f");
                break;
            default:
                buf_putchar_outbuffer(obp, *str);
                break;
        }
        str++;
    }
}

static int escape_sequence(void)
{
    int c;
    if ((c = buf_getchar()) == '\\') switch ((c = buf_getchar())) {
            case '/':
                c = '/';
                break;
            case '\\':
                c = '\\';
                break;
            case '"':
                c = '"';
                break;
            case 'b':
                c = '\b';
                break;
            case 'n':
                c = '\n';
                break;
            case 't':
                c = '\t';
                break;
            case 'f':
                c = '\f';
                break;
            case 'r':
                c = '\r';
                break;
            case 'u':
                buf_ungetchar(c);
                buf_ungetchar('\\');
                c = -2;
                break;
            default:
                buf_ungetchar(c);
                buf_ungetchar('\\');
                c = -1;
                break;
        }
    return c;
}

static void space_break(void)
{
    int c;
    while ((c = buf_getchar()) != EOF) {
        if (c == '\t' || c == '\n' || c == ' ' || c == '\r') continue;
        buf_ungetchar(c);
        break;
    }
    return;
}

static Json_T json_new(Json_Type_T type, void *val, char const *file, int line)
{
    Json_T jt = rmem_alloc(sizeof(*jt), file, line);
    jt->type  = type;
    jt->val   = val;
    return jt;
}

Json_T json_get_array(void)
{
    Json_T  jt;
    Array_T ar = rseq_new(0);
    int     c;

    while ((jt = json_parse_value()) != NULL) {
        rseq_addhi(ar, jt);

        space_break();
        if ((c = buf_getchar()) == ENTRY_DELIMITER)
            ;
        else if (c == ']')
            break;
        else
            buf_ungetchar(c);
    }

    /* empty */
    if (rseq_length(ar) == 0 && jt == NULL) {
        space_break();
        if ((c = buf_getchar()) == ']')
            ;
        else {
            fprintf(stderr, "[%d]\n", c);
            assert(0);
        }
    }

    return JSON_NEW(Array, ar);
}

/* atom key */
static int cmp_js_key_tree(void const *x, void const *y)
{
    int xn = strlen(((Json_T)x)->val);
    int yn = strlen(((Json_T)y)->val);
    if (xn > yn)
        return 1;
    else if (xn < yn)
        return -1;
    else
        return memcmp(((Json_T)x)->val, ((Json_T)y)->val, xn);
}

/* static int cmp_js_key(void const *x, void const *y) */
/* { */
/* return ((Json_T) x)->val != ((Json_T) y)->val; */
/* } */

static unsigned hash_js_key(void const *x)
{
    return (unsigned long)((Json_T)x)->val >> 2;
}

Json_T json_get_object(void)
{
    Json_T      jt  = NULL;
    Object_T    obj = rtable_new(0, cmp_js_key_tree, hash_js_key);
    int         c;
    void       *key;
    void const *k;

    while ((jt = json_parse_value()) != NULL && jt->type == String) {
        k = ratom_string(jt->val);
        FREE(jt->val);
        jt->val = (void *)k;
        key     = jt;

        space_break();
        if ((c = buf_getchar()) == KEY_VAL_DELIMITER)
            ;
        else
            assert(!"no match value");

        if ((jt = json_parse_value()) == NULL) assert(!"invalid type");
        rtable_put(obj, key, jt);

        space_break();
        if ((c = buf_getchar()) == ENTRY_DELIMITER)
            ;
        else if (c == '}')
            break;
        else
            buf_ungetchar(c);
    }

    /* empty */
    if (rtable_length(obj) == 0 && jt == NULL) {
        space_break();
        if ((c = buf_getchar()) == '}')
            ;
        else
            assert(0);
    } else if (rtable_length(obj) == 0 && jt != NULL) {
        assert(!"invalid");
    }

    return JSON_NEW(Object, obj);
}

static Json_T json_parse_value(void)
{
    int       c;
    int       i;
    Boolean_T b;
    int       maybe = False;
    char     *endp  = NULL;
    double    d;
    double   *dp;
    char     *str   = NULL;
    int       count = 0;

    space_break();
    switch ((c = buf_getchar())) {
        case '"':
            for (i = 0; (c = buf_getchar()) != EOF; i++) {
                if (c == '\\') {
                    buf_ungetchar(c);
                    c = escape_sequence();
                }
                if (c == -1)
                    assert(!"escape sequence fault");
                else if (c == -2 && i + 1 < BUFSIZ) { /* Unicode */
                    Internal_Buffer[i++] = buf_getchar();
                    Internal_Buffer[i]   = buf_getchar();
                } else
                    Internal_Buffer[i] = c;

                /* buf is full or end */
                if (i + 1 == BUFSIZ || c == '"') {
                    if (c != '"') i++;
                    if (!str) {
                        str = ALLOC(i + ((c == '"') ? 1 : 0)); /* for '\0' */
                        memcpy(str, Internal_Buffer, i);
                    } else {
                        RESIZE(str, count + i + ((c == '"') ? 1 : 0)); /* for '\0' */
                        memcpy(str + count, Internal_Buffer, i);
                    }
                    count += i;
                    i = -1;
                }
                if (c == '"') {
                    str[count] = '\0';
                    break;
                }
            }

            if (c != '"') {
                FREE(str);
                return NULL;
            }

            return JSON_NEW(String, str);
        case 't':
        case 'f':

            i                    = 0;
            Internal_Buffer[i++] = c;
            for (; i < 4 && (c = buf_getchar()) != EOF; i++) Internal_Buffer[i] = c;
            Internal_Buffer[i] = '\0';
            /* if (c == EOF) { ... } */

            if (memcmp(Internal_Buffer, True_Str, 5) == 0)
                maybe = True;
            else if (memcmp(Internal_Buffer, False_Str, 4) == 0 &&
                     (c = buf_getchar()) == 'e') {
                maybe = False;
            } else
                assert(!"bad string");

            NEW(b);
            *b = maybe;
            return JSON_NEW(Boolean, b);
        case 'n':

            i                    = 0;
            Internal_Buffer[i++] = c;
            for (; i < 4 && (c = buf_getchar()) != EOF; i++) Internal_Buffer[i] = c;
            Internal_Buffer[i] = '\0';

            if (memcmp(Internal_Buffer, Null_Str, 5) != 0) {
                assert(!"bad sting");
            }

            return JSON_NEW(Nil, NULL);
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '0':
        case '-':
        case '+':
        case '.':

            /* collect chars */
            i                    = 0;
            Internal_Buffer[i++] = c;
            while (i < DIGIT_STRS_LENGTH_MAX &&
                   ((c = buf_getchar()) == '-' || c == '+' || isdigit(c) || c == '.' ||
                    c == 'e' || c == 'E'))
                Internal_Buffer[i++] = c;
            Internal_Buffer[i] = '\0';

            buf_ungetchar(c);

            d = strtod(Internal_Buffer, &endp);
            /* if ((d = strtod(Internal_Buffer, &endp))) { */
            /* assert(!"underflow"); */
            /* } */
            if (errno == ERANGE) {
                assert(!"overflow");
            }
            if (endp == Internal_Buffer) {
                assert(!"string not work");
            }

            NEW(dp);
            *dp = d;
            return JSON_NEW(Digit, dp);
        case '[':
            return json_get_array();
        case '{':
            return json_get_object();
        default:
            /* assert(0); */
            buf_ungetchar(c);
            return NULL;
    }
    assert(0);
    return NULL;
}

/* static void json_print(void *, int); */
static void json_print_value(Output_Buffer_T *obp, void *x, int depth);

static void depth_space_print(Output_Buffer_T *obp, int depth)
{
    int i;
    for (i = 0; i < depth; i++) buf_putchar_outbuffer(obp, '\t');
}

/* static void app_rarray_print(void *x, void *cl) */
/* { */
/* json_print_value(x, *(int *)cl+1); */
/* } */

static void app_rarray_print(void *x, void *obp, void *cl)
{
    json_print_value(obp, x, *(int *)cl + 1);
}

static void boolean_print(Output_Buffer_T *obp, Boolean_T b)
{
    buf_putstr_outbuffer(obp, (*b == True) ? True_Str : False_Str);
    /* printf("%s", *b == True ? True_Str : False_Str); */
}

static int cmp_key(void const *x, void const *y)
{
    int xn = strlen((*(Json_T *)x)->val);
    int yn = strlen((*(Json_T *)y)->val);
    if (xn > yn)
        return 1;
    else if (xn < yn)
        return -1;
    else
        return strcmp((*(Json_T *)x)->val, (*(Json_T *)y)->val);
}

static void print_object(Output_Buffer_T *obp, Object_T obj, int depth)
{
    void **ar = rtable_to_array(obj, NULL);
    int    i, n, len = rtable_length(obj);
    qsort(ar, len, 2 * sizeof(*ar), cmp_key);
    for (i = n = 0; ar[i]; i += 2, n++) {
        buf_putchar_outbuffer(obp, '\n');
        depth_space_print(obp, depth + 1);
        buf_putchar_outbuffer(obp, '\"');
        print_str_unescape_seq(obp, ((Json_T)ar[i])->val);
        buf_putstr_outbuffer(obp, "\":");
        json_print_value(obp, ar[i + 1], depth + 1);
        if (n + 1 != len) buf_putchar_outbuffer(obp, ',');
    }
    FREE(ar);
}

/* static void rseq_map(Array_T seq,  */
/* void apply(void *x, void *cl), void *cl) */
/* { */
/* int i, n = rseq_length(seq); */
/* for (i = 0; i < n; i++) { */
/* apply(rseq_get(seq, i), cl); */
/* if (i+1 != n) */
/* buf_putchar_outbuffer(obp, ','); */
/* } */
/* } */

static void rseq_print(Array_T seq, void app_print(void *x, void *obp, void *cl),
                       void *obp, void *cl)
{
    int i, n = rseq_length(seq);
    for (i = 0; i < n; i++) {
        app_print(rseq_get(seq, i), obp, cl);
        if (i + 1 != n) buf_putchar_outbuffer(obp, ',');
    }
}

static void json_print_value(Output_Buffer_T *obp, void *x, int depth)
{
    char   buf[DIGIT_STRS_LENGTH_MAX];
    Json_T jt = x;
    assert(jt);
    depth_space_print(obp, depth);
    switch (jt->type) {
        case Array:
            buf_putstr_outbuffer(obp, "[\n");
            depth_space_print(obp, depth);
            rseq_print((Array_T)jt->val, app_rarray_print, obp, &depth);
            buf_putchar_outbuffer(obp, '\n');
            depth_space_print(obp, depth);
            buf_putstr_outbuffer(obp, "]\n");
            break;
        case Object:
            buf_putstr_outbuffer(obp, "{\n");
            depth_space_print(obp, depth);
            print_object(obp, (Object_T)jt->val, depth);
            /* rtable_map((Object_T) jt->val, app_obj_print, &depth); */
            buf_putchar_outbuffer(obp, '\n');
            depth_space_print(obp, depth);
            buf_putstr_outbuffer(obp, "}\n");
            break;
        case String:
            buf_putchar_outbuffer(obp, '\"');
            print_str_unescape_seq(obp, jt->val);
            buf_putchar_outbuffer(obp, '\"');
            break;
        case Digit:
            snprintf(buf, DIGIT_STRS_LENGTH_MAX, "%g", *(double *)jt->val);
            buf_putstr_outbuffer(obp, buf);
            /* printf("%g", *(double *) jt->val); */
            break;
        case Boolean:
            boolean_print(obp, jt->val);
            break;
        case Nil:
            buf_putstr_outbuffer(obp, "null");
            break;
        case Error:
        default:
            assert(0);
    }
}

char *json_print(void *x, int depth)
{
    Output_Buffer_T ob;
    memset(&ob, '\0', sizeof(ob));
    json_print_value(&ob, x, depth);
    buf_putchar_outbuffer(&ob, '\0');
    /* buf_flush_outbuffer(&ob, stdout); */
    /* fprintf(stderr, "--- Outbuffer ---\noffset: %ld\nlength: %ld\n--- buffend ---\n",
     */
    /* ob.offset, ob.length); */
    return ob.buf;
}

Json_T json_parse(FILE *fp)
{
    Global_Input_Buffer.in = fp;
    return json_parse_value();
}

static void rarray_vfree(Array_T ar)
{
    int   n = rseq_length(ar);
    int   i;
    void *x;
    for (i = 0; i < n; i++) {
        x = rseq_get(ar, i);
        json_dispose((Json_T *)&x);
    }
}

static void object_vfree(void const *k, void **val, void *cl)
{
    /* free json key, key string is atom */
    rmem_free((void *)k, __FILE__, __LINE__);
    json_dispose((Json_T *)val);
}

void json_dispose(Json_T *jt)
{
    switch ((*jt)->type) {
        case Array:
            rarray_vfree((*jt)->val);
            rseq_free((Array_T *)&(*jt)->val);
            break;
        case Object:
            rtable_map((*jt)->val, object_vfree, NULL);
            rtable_free((Object_T *)&(*jt)->val);
            break;
        case String:
        case Digit:
        case Boolean:
            FREE((*jt)->val);
            break;
        case Nil:
            break;
        case Error:
        default:
            assert(0);
    }
    FREE(*jt);
}

Json_T json_object_get(Json_T jt, void const *key)
{
    assert(jt);
    return rtable_get((Object_T)jt->val, key);
}

Json_T json_object_put(Json_T jt, void const *key, Json_T val)
{
    assert(jt);
    return rtable_put((Object_T)jt->val, key, val);
}

int json_object_length(Json_T jt)
{
    assert(jt);
    return rtable_length((Object_T)jt->val);
}

void json_object_map(Json_T jt, void app(void const *, void **, void *cl), void *cl)
{
    rtable_map((Object_T)jt->val, app, cl);
}

Json_T json_object_new(int hint, int cmp(void const *x, void const *y),
                       unsigned hash(void const *x))
{
    Object_T obj =
        rtable_new(hint, cmp ? cmp : cmp_js_key_tree, hash ? hash : hash_js_key);
    return JSON_NEW(Object, obj);
}

Json_T json_rarray_get(Json_T jt, int i)
{
    assert(jt);
    return rseq_get((Array_T)jt->val, i);
}

Json_T json_rarray_put(Json_T jt, int i, Json_T val)
{
    assert(jt);
    return rseq_put((Array_T)jt->val, i, val);
}

int json_rarray_length(Json_T jt)
{
    assert(jt);
    return rseq_length((Array_T)jt->val);
}

Json_T json_rarray_addlo(Json_T array, Json_T jt)
{
    assert(array);
    return rseq_addlo((Array_T)array->val, jt);
}

Json_T json_rarray_addhi(Json_T array, Json_T jt)
{
    assert(array);
    return rseq_addhi((Array_T)array->val, jt);
}

Json_T json_rarray_remlo(Json_T array)
{
    assert(array);
    return rseq_remlo((Array_T)array->val);
}

Json_T json_rarray_remhi(Json_T array)
{
    assert(array);
    return rseq_remhi((Array_T)array->val);
}

Json_T json_rarray_new(int hint)
{
    Array_T seq = rseq_new(hint);
    return JSON_NEW(Array, seq);
}

Json_T json_string_new(char *str)
{
    assert(str);
    return JSON_NEW(String, str);
}

Json_T json_digit_new(double d)
{
    double *dp = ALLOC(sizeof(*dp));
    *dp        = d;
    return JSON_NEW(Digit, dp);
}

Json_T json_null_new(void) { return JSON_NEW(Nil, NULL); }

Json_T json_boolean_new(int b)
{
    assert(b == False || b == True);
    int *bp = ALLOC(sizeof(*bp));
    *bp     = (b == False) ? False : True;
    return JSON_NEW(Boolean, bp);
}

/* int main(int argc, char *argv[]) */
/* { */
/* Json_T jt; */

/* jt = json_parse(stdin); */
/* printf("obj length: %d\n", json_object_length(jt)); */

/* Json_T topics_key; */
/* NEW(topics_key); */
/* topics_key->type = String; */
/* topics_key->val = (void *) ratom_string("topics"); */
/* Json_T topics = json_object_get(jt, topics_key); */
/* if (topics) { */
/* if (topics->type == Array) */
/* printf("%d\n", json_rarray_length(topics)); */
/* else  */
/* printf("%d\n", topics->type); */
/* for (int i = 0; i < json_rarray_length(topics); i++) { */
/* printf("%03d - %p - ", i, json_rarray_get(topics, i)); */
/* printf("%d\n", json_rarray_get(topics, i)->type); */
/* json_print(json_rarray_get(topics, i), 0); */
/* } */
/* } else  */
/* printf("empty;"); */

/* [> json_print(jt, 0); <] */
/* putchar('\n'); */

/* #ifdef MEM_CHK */
/* rmem_print_allocated(); */
/* rmem_print_freed(); */
/* #endif */
/* sleep(90); */

/* return 0; */
/* } */
