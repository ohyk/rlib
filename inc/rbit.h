#ifndef RBIT_H
#define RBIT_H 
#include "rdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define T RBit_T
typedef struct T *T;

/*====================================================================*/
/*
 * Base bit set function:
 *  rbit_new():
 *      Create new set by length 
 *  rbit_length:
 *      get set length 
 *  rbit_count():
 *      count bit is 1 in set
 *
 * Return:
 *      rbit_new()       --> Bit_T (length [0, length))
 *      rbit_length()    --> length
 *      rbit_count()     --> count
 * 
 * Checked Runtime Error:
 *      - length < 0    --> RAssertFailed
 *      - set is NULL   --> RAssertFailed
 * 
 * Exception:
 *      - rbit_new() allocate failed     --> RMemFailed
 * 
 */
/*====================================================================*/
extern RLIB_EXPORT_FLAG T rbit_new(int length);
extern RLIB_EXPORT_FLAG int rbit_length(T set);
extern RLIB_EXPORT_FLAG int rbit_count(T set);

/*====================================================================*/
/*
 * Freed *set and set *set be NULL
 * 
 * Checked Runtime Error:
 *      - set is NULL   --> RAssertFailed
 *      - *set is NULL  --> RAssertFailed
 * 
 * Unchecked Runtime Error:
 * 
 * Exception:
 * 
 */
/*====================================================================*/
extern RLIB_EXPORT_FLAG void rbit_free(T *set);

/*====================================================================*/
/*
 * Get/Set bit of set
 *
 * Return:
 *      rbit_get()       --> bit value by n in set
 *      rbit_put()       --> old bit value
 * 
 * Checked Runtime Error:
 *      - set is NULL               --> RAssertFailed
 *      - bit != 1 && bit != 0      --> RAssertFailed
 *      - n > 0 || n < length-1     --> RAssertFailed
 * 
 * Unchecked Runtime Error:
 * 
 * Exception:
 * 
 */
/*====================================================================*/
extern RLIB_EXPORT_FLAG int rbit_get(T set, int n);
extern RLIB_EXPORT_FLAG int rbit_put(T set, int n, int bit);

/*====================================================================*/
/*
 * Bits operations functions:
 *  rbit_clear():
 *      set bit to 0 by [lo, hi]
 *  rbit_set():
 *      set bit to 1 by [lo, hi]
 *  rbit_not():
 *      set !bit by [lo, hi]
 * 
 * Checked Runtime Error:
 *      - set is NULL                   --> RAssertFailed
 *      - lo < 0 || hi < 0              --> RAssertFailed
 *      - lo > hi                       --> RAssertFailed
 *      - lo >= length or hi >= length  --> RAssertFailed
 * 
 * Unchecked Runtime Error:
 * 
 * Exception:
 * 
 */
/*====================================================================*/
extern RLIB_EXPORT_FLAG void rbit_clear(T set, int lo, int hi);
extern RLIB_EXPORT_FLAG void rbit_set(T set, int lo, int hi);
extern RLIB_EXPORT_FLAG void rbit_not(T set, int lo, int hi);

extern RLIB_EXPORT_FLAG int rbit_lt(T s, T t);
extern RLIB_EXPORT_FLAG int rbit_eq(T s, T t);
extern RLIB_EXPORT_FLAG int rbit_leq(T s, T t);

extern RLIB_EXPORT_FLAG void rbit_map(T set, void apply(int n, int bit, void *cl), void *cl);

extern RLIB_EXPORT_FLAG T rbit_union(T s, T t);
extern RLIB_EXPORT_FLAG T rbit_inter(T s, T t);
extern RLIB_EXPORT_FLAG T rbit_minus(T s, T t);
extern RLIB_EXPORT_FLAG T rbit_diff(T s, T t);

#ifdef __cplusplus
}
#endif

#undef T

#endif /* ifndef RBIT_H */
