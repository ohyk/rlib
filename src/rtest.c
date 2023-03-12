#include "rtest.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "rarena.h"
#include "rassert.h"
#include "rrandom.h"

/*====================================================================*/
/*
 * RTEST module environment
 *
 * Use arena module allocated memory,
 * only way get Rtest_Env is get_env().
 */
/*====================================================================*/
typedef struct RTestEnv_T *RTestEnv_T;
struct RTestEnv_T
{
    int      rand_seed;
    RArena_T arena;
};

static struct RTestEnv_T Rtest_Env;

static inline RTestEnv_T get_env(void)
{
    if (!Rtest_Env.arena) {
        srand(time(NULL));
        Rtest_Env.rand_seed = 1;
        Rtest_Env.arena     = rarena_new();
    }
    return &Rtest_Env;
}

/* --- Memory Part --- */
void *rtest_mem_alloc(long nbytes, char const *file, int line)
{
    RTestEnv_T env = get_env();
    return rarena_alloc(env->arena, nbytes, file, line);
}

void rtest_mem_dispose(void)
{
    RTestEnv_T env = get_env();
    rarena_dispose(&env->arena);
}

/* --- Generate Part --- */
int *rtest_new_int(int i, char const *file, int line)
{
    int *iptr = rtest_mem_alloc(sizeof(*iptr), file, line);
    *iptr     = i;
    return iptr;
}

int *rtest_make_range(int s, int e, int step, char const *file, int line)
{
    rassert(s < e);
    rassert(step > 0);
    unsigned len = (e - s) / step;
    int     *res;
    int      i;
    rassert(len != 0);
    res = rtest_mem_alloc(len * sizeof(*res), file, line);
    for (i = 0; s < e; i++, s += step) res[i] = s;
    return res;
}

char *rtest_new_str(char const *str, char const *file, int line)
{
    int   len;
    char *dstr;
    rassert(str);
    len  = strlen(str);
    dstr = rtest_mem_alloc(len + 1, file, line);
    memcpy(dstr, str, len + 1);
    return dstr;
}

#define CS_LEN(s) (sizeof(s) - 1)
static char const Constants_String[]        = LOWERS_STR UPPERS_STR DIGITS_STR;
static int const Constants_String_Lengths[] = {
    CS_LEN(LOWERS_STR UPPERS_STR DIGITS_STR), /* alnum */
    CS_LEN(LOWERS_STR UPPERS_STR),            /* alpha */
    CS_LEN(LOWERS_STR),                       /* Word */
    CS_LEN(LOWERS_STR),                       /* lower */
    CS_LEN(UPPERS_STR),                       /* upper */
    CS_LEN(DIGITS_STR),                       /* digit */
};

int rtest_make_random_str_with_str(char *str, int length, RTestMakeStrMode_T mode)
{
    rassert(str);
    rassert(length > 0);
    char const *c = Constants_String;
    int         i;
    switch (mode) {
        case Rtest_Make_Str_Mode_Digit:
            c += Constants_String_Lengths[Rtest_Make_Str_Mode_Upper];
        case Rtest_Make_Str_Mode_Upper:
            c += Constants_String_Lengths[Rtest_Make_Str_Mode_Lower];
        case Rtest_Make_Str_Mode_Lower:
        case Rtest_Make_Str_Mode_Word:
        case Rtest_Make_Str_Mode_Alpha:
        case Rtest_Make_Str_Mode_Alnum:
            break;
        case Rtest_Make_Str_Mode_Error:
        default:
            rassert(!"Rtest make string mode unknow");
    }

    i = 0;
    if (mode == Rtest_Make_Str_Mode_Word) {
        str[i++] =
            (c + Constants_String_Lengths[Rtest_Make_Str_Mode_Lower])[rtest_random_int(
                Constants_String_Lengths[Rtest_Make_Str_Mode_Upper])];
    }
    for (; i < length - 1; i++)
        str[i] = c[rtest_random_int(Constants_String_Lengths[mode])];
    str[i] = '\0';
    return i;
}

char *rtest_make_random_str(int length, RTestMakeStrMode_T mode, char const *file,
                            int line)
{
    rassert(length > 0);
    char *res = NULL;
    TRY   res = rtest_mem_alloc(length + 1, file, line);
    rtest_make_random_str_with_str(res, length + 1, mode);
    EXCEPT(RArenaFailed)
    rexcept_raise(&RArenaFailed, file, line);
    END_TRY;
    return res;
}

int *rtest_make_random_ints(int length, char const *file, int line)
{
    rassert(length > 0);
    int *iar = rtest_mem_alloc(length * sizeof(int), file, line);
    int  i;

    for (i = 0; i < length; i++) iar[i] = rand();

    return iar;
}

static inline void swap(int *x, int *y)
{
    int c = *x;
    *x    = *y;
    *y    = c;
}

int *rtest_make_notrepeat_ints(int hi, char const *file, int line)
{
    rassert(hi > 0);
    int *iar = rtest_mem_alloc(sizeof(*iar) * hi, file, line);
    int  i;
    for (iar[0] = 0, i = 1; i < hi; i++) {
        iar[i] = i;
        swap(&iar[i], &iar[rtest_random_int(i)]);
    }
    return iar;
}

int32_t rtest_random_int32()
{
    get_env();
    return rrandom_rand32();
}

int32_t rtest_uniform_int32_rl(int32_t s, int32_t e)
{
    rassert(s < e);
    return rrandom_uniform_int32(s, e);
}

int rtest_random_int(int n)
{
    rassert(n > 0);
    return rtest_uniform_int32_rl(0, n);
}

int rtest_random_int_rl(int r, int l)
{
    rassert(r < l);
    int diff = l - r;
    return rtest_random_int(diff + 1) + r;
}

int *rtest_make_random_int(int n, char const *file, int line)
{
    int *iptr = rtest_mem_alloc(sizeof(*iptr), file, line);
    *iptr     = rtest_random_int(n);
    return iptr;
}

int *rtest_make_uniform_ints(int len, int n, char const *file, int line)
{
    rassert(len > 0);
    int *iar = rtest_mem_alloc(sizeof(*iar) * len, file, line);
    int  i;
    for (i = 0; i < len; i++) iar[i] = rtest_random_int(n);
    return iar;
}

int rtest_set_get_test(void *adt, void (*set)(void *adt, void *i, void *v),
                       void *(*get)(void *adt, void *i))
{
    int  i;
    int *x;
    int  res;

    res = 0; /* ok */
    /* set */
    for (i = 0; i < 10; i++) {
        set(adt, &i, NEW_INT(i));
    }

    for (i = 0; i < 10; i++) {
        x = get(adt, &i);
        if (*x != i) {
            res = 1; /* bad */
            break;
        }
    }

    return res; /* check status */
}

int rtest_remove_test(void *adt, void (*set)(void *adt, void *i, void *v),
                      void *(*get)(void *adt, void *i), void *(*rem)(void *adt, void *i))
{
    int i;
    int res;

    res = rtest_set_get_test(adt, set, get);

    if (res != 0) {
        res = 1;
    } else {
        for (i = 0; i < 10; i++) {
            rem(adt, &i);
            if (get(adt, &i)) {
                res = 1;
                break;
            }
        }
    }
    return res;
}

#ifdef ARENA_DEBUG
void rtest_mem_print(void)
{
    RTestEnv_T env = get_env();
    rarena_print(env->arena);
}
#endif

#include <stdio.h>
void rtest_print_array(void *ar, int ar_len, int line_len, char *delm,
                       void (*pte)(void *x, void *cl_fmt), void *cl_fmt)
{
    rassert(ar && ar_len > 0);
    rassert(line_len > 0);

    int i;

    for (i = 0; i < ar_len; i++) {
        pte(ar + i, cl_fmt);
        if (i + 1 != ar_len) printf("%s", delm);
        if ((i + 1) % line_len == 0) printf("\n");
    }
    if (i % line_len != 0) printf("\n");
}

/*====================================================================*/
/*
 * Test string to X - string to something ADT with app
 *
 * Get string from in_fd to app, string is 'quit' or 'exit' to return.
 * Skip string start blank and space, replace tail '\n' to '\0'.
 *
 * If in_fd is NULL, then set stdin.
 *
 * Checked Runtime Error:
 *      app is NULL         --> RAssertFailed
 *
 */
/*====================================================================*/
static void del_ln(char *str, int len)
{
    int i;
    for (i = 0; i < len && str[i] && str[i] != '\n'; i++) continue;
    str[i] = '\0';
}

#include <ctype.h>
void rtest_test_string_to_x(FILE *in_fp, void (*app)(char const *, int, void *cl),
                            void *cl)
{
    char line[BUFSIZ];
    int  i;

    rassert(app);

    if (in_fp == NULL) in_fp = stdin;

    while (fgets(line, BUFSIZ, in_fp) != NULL) {
        if (strncmp("quit", line, 4) == 0 || strncmp("exit", line, 4) == 0) break;
        for (i = 0; i < BUFSIZ && line[i] && (isspace(line[i]) || isblank(line[i])); i++)
            continue;                                /* skip space and blank */
        if (i < BUFSIZ && line[i] == '\0') continue; /* empty line */

        del_ln(line + i, BUFSIZ - i);
        app(line + i, BUFSIZ - i, cl);
    }
}
