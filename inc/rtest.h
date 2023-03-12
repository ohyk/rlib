#ifndef RTEST_H
#define RTEST_H 
#include "rdef.h"
#include <stdint.h>


#define RTEST_STATUS_STR(x) ((x) ? "OK" : "ERROR")

/*====================================================================*/
/*
 * For ADT tset
 * rtest_set_get_test() --- Test set and get
 * rtest_remove_test() --- Test set get and remove
 *
 * User defined set() get() rem()
 *
 * Return: 
 *      0   --> Success
 *      1   --> Failed
 */
/*====================================================================*/
extern RLIB_EXPORT_FLAG int rtest_set_get_test(void *adt, 
                                               void (*set)(void *adt, void *i, void *v),
                                               void *(*get)(void *adt, void *i));

extern RLIB_EXPORT_FLAG int rtest_remove_test(void *adt, 
                                              void (*set)(void *adt, void *i, void *v),
                                              void *(*get)(void *adt, void *i), 
                                              void *(*rem)(void *adt, void *i));

/*====================================================================*/
/*
 * Create integer with i
 */
/*====================================================================*/
extern RLIB_EXPORT_FLAG int *rtest_new_int(int i, char const *file, int line);
#define NEW_INT(i) \
    rtest_new_int((i), __FILE__, __LINE__)

/*====================================================================*/
/*
 * Generate integers array with range
 *
 * Range: s...e-1
 *
 * Step: step
 *
 * Array Length: (e-s)/step
 *
 * Checked Runtime Error:
 *      - s > e             --> RAssertFailed
 *      - step <= 0         --> RAssertFailed
 *      - (e-s)/step == 0   --> RAssertFailed
 */
/*====================================================================*/
extern RLIB_EXPORT_FLAG int *rtest_make_range(int s, int e, int step,
                                              char const *file, int line);
#define MAKE_RANGE(s, e, step) \
    rtest_make_range((s), (e), (step), __FILE__, __LINE__)
#define MAKE_RANGE1(s, e) \
    MAKE_RANGE((s), (e), 1)

/*====================================================================*/
/*
 * Copy str return new string 
 *
 * Checked Runtime Error:
 *      - str is NULL       --> RAssertFailed
 */
/*====================================================================*/
extern RLIB_EXPORT_FLAG char *rtest_new_str(char const *str, 
                                            char const *file, int line);
#define NEW_STR(str) \
    rtest_new_str((str), __FILE__, __LINE__)

/*====================================================================*/
/*
 * Generate random string with length
 *
 * Mode: 
 *      - Rtest_Make_Str_Mode_Alpha: lower and upper, e.g.: "AsDF"
 *      - Rtest_Make_Str_Mode_Word: word, e.g: "Wsad"
 *      - Rtest_Make_Str_Mode_Lower: lower, e.g.: "asdf"
 *      - Rtest_Make_Str_Mode_Upper: upper, e.g.: "ASDF"
 *      - Rtest_Make_Str_Mode_Digit: digit, e.g: "3155"
 * 
 * Length: length + 1 (with terminator '\0')
 *
 * Checked Runtime Error:
 *      - length <= 0                   --> RAssertFailed
 *      - mode is invaild && error      --> RAssertFailed
 */
/*====================================================================*/
#define LOWERS_STR "abcdefghijklmnopqrstuvwxyz"
#define UPPERS_STR "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define DIGITS_STR "0123456789"

typedef enum {  Rtest_Make_Str_Mode_Alnum, Rtest_Make_Str_Mode_Alpha, 
                Rtest_Make_Str_Mode_Word, Rtest_Make_Str_Mode_Lower, 
                Rtest_Make_Str_Mode_Upper, Rtest_Make_Str_Mode_Digit, 
                Rtest_Make_Str_Mode_Error 
} RTestMakeStrMode_T;
extern RLIB_EXPORT_FLAG char *rtest_make_random_str(int length, 
                                                    RTestMakeStrMode_T mode,
                                                    char const *file, int line);
extern RLIB_EXPORT_FLAG int rtest_make_random_str_with_str(char *str, int length, 
                                                           RTestMakeStrMode_T mode);
#define MAKE_RANDOM_STR(len, mode) \
    rtest_make_random_str((len), (mode), __FILE__, __LINE__)
#define MAKE_RANDOM_STR_LOWER(len) \
    MAKE_RANDOM_STR((len), Rtest_Make_Str_Mode_Lower)
#define MAKE_RANDOM_STR_UPPER(len) \
    MAKE_RANDOM_STR((len), Rtest_Make_Str_Mode_Upper)
#define MAKE_RANDOM_STR_ALPHA(len) \
    MAKE_RANDOM_STR((len), Rtest_Make_Str_Mode_Alpha)
#define MAKE_RANDOM_STR_DIGIT(len) \
    MAKE_RANDOM_STR((len), Rtest_Make_Str_Mode_Digit)
#define MAKE_RANDOM_STR_WORD(len) \
    MAKE_RANDOM_STR((len), Rtest_Make_Str_Mode_Word)
#define MAKE_RANDOM_STR_ALNUM(len) \
    MAKE_RANDOM_STR((len), Rtest_Make_Str_Mode_Alnum)

/*====================================================================*/
/*
 * Generate random integers with length
 *
 * Checked Runtime Error:
 *      - length <= 0       --> RAssertFailed
 */
/*====================================================================*/
extern RLIB_EXPORT_FLAG int *rtest_make_random_ints(int length, 
                                                    char const *file, int line);
#define MAKE_RAND_INTS(len) \
    rtest_make_random_ints((len), __FILE__, __LINE__)

/*====================================================================*/
/*
 * Generate not repate random integers array with high
 *
 * Array Range: 0...hi-1
 *
 * Length: hi
 *
 * Checked Runtime Error:
 *      - hi <= 0           --> RAssertFailed
 */
/*====================================================================*/
extern RLIB_EXPORT_FLAG int *rtest_make_notrepeat_ints(int hi, 
                                                       char const *file, int line);
#define MAKE_NOT_REPEAT_INTS(hi) \
    rtest_make_notrepeat_ints((hi), __FILE__, __LINE__)

/*====================================================================*/
/*
 * Generate random integer of 32 bits
 *
 * rtest_random_int32():
 *      generate a random 32 bits integer number 
 * rtest_uniform_int32_rl(s,e):
 *      generate a random 32 bits integer number in [s, e)
 */
/*====================================================================*/
#define RAND32_MAX 0x7FFFFFFF
extern RLIB_EXPORT_FLAG int32_t rtest_random_int32();
extern RLIB_EXPORT_FLAG int32_t rtest_uniform_int32_rl(int32_t s, int32_t e);

/*====================================================================*/
/*
 * rtest_random_int():
 *      generate random integer, range of 0...n-1
 *
 * rtest_make_random_int()
 *      create random integer use rtest_random_int()
 *
 * Checked Runtime Error:
 *      - n <= 0        --> RAssertFailed
 */
/*====================================================================*/
extern RLIB_EXPORT_FLAG int rtest_random_int(int n);
extern RLIB_EXPORT_FLAG int *rtest_make_random_int(int n, 
                                                   char const *file, int line);
#define MAKE_RANDOM_INT(n) \
    rtest_make_random_int((n), __FILE__, __LINE__)

extern RLIB_EXPORT_FLAG int rtest_random_int_rl(int r, int l);

/*====================================================================*/
/*
 * Generate random integer array use rtest_random_int()
 *
 * Rrange: 0...n-1
 *
 * Length: len
 *
 * Checked Runtime Error: 
 *      - len <= 0      --> RAssertFailed
 */
/*====================================================================*/
extern RLIB_EXPORT_FLAG int *rtest_make_uniform_ints(int len, int n, 
                                                     char const *file, int line);
#define MAKE_UNIFORM_INTS(len, n) \
    rtest_make_uniform_ints((len), (n), __FILE__, __LINE__)

/*====================================================================*/
/*
 * Test string to X - string to something ADT with app
 *
 * Get string from in_fd to app, string is 'quit' or 'exit' to return.
 * Skip string start blank and space, replace tail '\n' to '\0'.
 * Skip empty line.
 *
 * If in_fd is NULL, then set stdin.
 * 
 * Checked Runtime Error:
 *      app is NULL         --> RAssertFailed
 *
 */
/*====================================================================*/
#include <stdio.h>
extern RLIB_EXPORT_FLAG void rtest_test_string_to_x(FILE *in_fp, 
                                                    void (*app)(char const *, int, void *),
                                                    void *cl);

/* print */
extern RLIB_EXPORT_FLAG void rtest_print_array(void *ar, int ar_len,
                                               int line_len, char *delm,
                                               void (*pte)(void *x, void *cl_fmt),
                                               void *cl_fmt);

/*====================================================================*/
/*
 * Allocate memory
 *
 * Checked Runtime Error:
 *      - nbytes <= 0               --> RAssertFailed
 *      - allocate failed           --> RArenaFailed
 *      - Rtest_Env init failed     --> RArenaNewFailed
 * 
 * Unchecked Runtime Error:
 * 
 * Exception:
 * 
 */
/*====================================================================*/
extern RLIB_EXPORT_FLAG void *rtest_mem_alloc(long nbytes,
                                              char const *file, int line);
extern RLIB_EXPORT_FLAG void rtest_mem_dispose(void);
#ifdef ARENA_DEBUG
extern RLIB_EXPORT_FLAG void rtest_mem_print(void);
#endif

#define RTEST_ALLOC(nbytes) \
    rtest_mem_alloc((nbytes), __FILE__, __LINE__)

#endif /* ifndef RTEST_H */
