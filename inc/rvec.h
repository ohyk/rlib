/*========================================================================*/
/*
 * Based on eteran/c-vector
 */
/*========================================================================*/
#ifndef RVEC_H
#define RVEC_H

#include <stdlib.h>
#include <string.h>

#include "rassert.h"

#ifndef rvec_clib_free
#define rvec_clib_free free
#endif /* ifndef rvec_clib_free */
#ifndef rvec_clib_malloc
#define rvec_clib_malloc malloc
#endif /* ifndef rvec_clib_malloc */
#ifndef rvec_clib_calloc
#define rvec_clib_calloc calloc
#endif /* ifndef rvec_clib_calloc */
#ifndef rvec_clib_realloc
#define rvec_clib_realloc realloc
#endif /* ifndef rvec_clib_realloc */

typedef void (*rvec_elem_destructor_t)(void *elem);

/*========================================================================*/
/*
 * The vector type used in this library.
 */
/*========================================================================*/
#define rvec_vector_type(type) type *

/*========================================================================*/
/*
 * Gets the current capacity of the vector.
 *
 * Parameter: vec -> the vector
 *
 * Return: capacity, capacity as a size_t
 *
 * Checked Runtime Error: None
 * Unchecked Runtime Error: None
 * Exception: None
 *
 */
/*========================================================================*/
#define rvec_capacity(vec) ((vec) ? ((size_t *)(vec))[-1] : (size_t)0)

/*========================================================================*/
/*
 * Gets the current size of the vector.
 *
 * Parameter: vec -> the vector
 *
 * Return: size, size as a size_t
 *
 * Checked Runtime Error: None
 * Unchecked Runtime Error: None
 * Exception: None
 *
 */
/*========================================================================*/
#define rvec_size(vec) ((vec) ? ((size_t *)(vec))[-2] : (size_t)0)

/*========================================================================*/
/*
 * Set the element destructor function uesd to clean up removed elements.
 *
 * Parameter:
 *      vec -> the vector
 *      elem_destructor_fn -> function pointer of type rvec_elem_destructor_t
 *
 * Return: rvec_elem_destructor_t
 *      elem_destructor_fn -> ok
 *      NULL               -> error
 *
 * Checked Runtime Error: None
 * Unchecked Runtime Error: None
 * Exception: None
 *
 */
/*========================================================================*/
#define rvec_set_elem_destructor(vec, elem_destructor_fn)                               \
    do {                                                                                \
        if (!(vec)) {                                                                   \
            rvec_grow((vec), 0)                                                         \
        }                                                                               \
        ((rvec_elem_destructor_t *)&(((size_t *)(vec))[-2]))[-1] = (elem_destructor_fn) \
    } while (0)

/*========================================================================*/
/*
 * Get the element destructor function used to clean up elements.
 *
 * Parameter: vec -> the vector
 *
 * Return: rvec_elem_destructor_t
 *      elem_destructor_fn -> ok
 *      NULL               -> error
 *
 * Checked Runtime Error: None
 * Unchecked Runtime Error: None
 * Exception: None
 *
 */
/*========================================================================*/
#define rvec_elem_destructor(vec) \
    ((vec) ? (((rvec_elem_destructor_t *)&(((size_t *)(vec))[-2]))[-1]) : NULL)

/*========================================================================*/
/*
 * Returns non-zero if the vector is empty.
 *
 * Parameter: vec -> the vector
 *
 * Return: bool
 *      true  -> empty
 *      false -> non-empty
 *
 * Checked Runtime Error: None
 * Unchecked Runtime Error: None
 * Exception: None
 *
 */
/*========================================================================*/
#define rvec_empty(vec) (rvec_size(vec) == 0)

/*========================================================================*/
/*
 * Requests that the vector capacity be at least enough to contain n elements.
 * If n is greater than current vector capacity, the function causes the
 * container to reallocate its storage increasing its capacity to n (or
 * greater).
 *
 * Parameter:
 *      vec -> the vector
 *      n   -> minimum capacity for the vector
 *
 * Return: void
 *
 * Checked Runtime Error:
 *      necessary allocated malloc return NULL -> Assert_Failed
 *
 * Unchecked Runtime Error: None
 * Exception: None
 *
 */
/*========================================================================*/
#define rvec_reserve(vec, capacity)         \
    do {                                    \
        size_t rv_cap = rvec_capacity(vec); \
        if (rv_cap < (capacity)) {          \
            rvec_grow((vec), (capacity));   \
        }                                   \
    } while (0)

/*========================================================================*/
/*
 * Removes the element at index i from the vector.
 *
 * Parameter:
 *      vec -> the vector
 *      i   -> index of element to remove
 *
 * Return: void
 * Checked Runtime Error: None
 * Unchecked Runtime Error: None
 * Exception: None
 *
 */
/*========================================================================*/
#define rvec_erase(vec, i)                                   \
    do {                                                     \
        if ((vec)) {                                         \
            size_t const rv_sz = rvec_size(vec);             \
            if ((i) < rv_sz) {                               \
                rvec_set_size((vec), rv_sz - 1);             \
                memmove((vec) + (i), (vec) + (i) + 1,        \
                        sizeof(*(vec)) * (rv_sz - 1 - (i))); \
            }                                                \
        }                                                    \
    } while (0)

/*========================================================================*/
/*
 * Frees all memory associated with the vector.
 *
 * Parameter: vec -> the vector
 *
 * Return: void
 * Checked Runtime Error: None
 * Unchecked Runtime Error: None
 * Exception: None
 *
 */
/*========================================================================*/
#define rvec_free(vec)                                                                 \
    do {                                                                               \
        if ((vec)) {                                                                   \
            size_t *p1 =                                                               \
                (size_t *)&(((rvec_elem_destructor_t *)&(((size_t *)(vec))[-2]))[-1]); \
            rvec_elem_destructor_t e_destructor = rvec_elem_destructor((vec));         \
            if (e_destructor) {                                                        \
                size_t i;                                                              \
                for (i = 0; i < rvec_size(vec); ++i) e_destructor(&vec[i]);            \
            }                                                                          \
            rvec_clib_free(p1);                                                        \
        }                                                                              \
    } while (0)

/*========================================================================*/
/*
 * Returns an iterator to first element of the vector.
 *
 * Parameter: vec -> the vector
 *
 * Return: pointer
 *      first -> ok
 *      NULL  -> error
 *
 * Checked Runtime Error: None
 * Unchecked Runtime Error: None
 * Exception: None
 *
 */
/*========================================================================*/
#define rvec_begin(vec) (vec)

/*========================================================================*/
/*
 * Returns an iterator to one past the last element fo the vector.
 *
 * Parameter: vec -> vector
 *
 * Return: pointer
 *      last -> ok
 *      NULL -> empty
 *
 * Checked Runtime Error: None
 * Unchecked Runtime Error: None
 * Exception: None
 *
 */
/*========================================================================*/
#define rvec_end(vec) ((vec) ? &((vec)[rvec_size(vec)]) : NULL)

/* user request to use logarithmic growth algorithm  */
#ifdef RVEC_LOGARITHMIC_GROWTH
/*========================================================================*/
/*
 * Returns an the computed size in next vector grow size is increased by
 * multiplication of 2.
 *
 * Parameter: size -> current size
 *
 * Return: size, size after next vector grow
 *
 * Checked Runtime Error: None
 * Unchecked Runtime Error: None
 * Exception: None
 *
 */
/*========================================================================*/
#define rvec_compute_next_grow(size) ((size) ? ((size) << 1) : 1)
#else
/*========================================================================*/
/*
 * Returns an the computed size in next vector grow size is increased by 1.
 *
 * Parameter:  size -> current size
 *
 * Return: size, size after next vector grow
 *
 * Checked Runtime Error: None
 * Unchecked Runtime Error: None
 * Exception: None
 *
 */
/*========================================================================*/
#define rvec_compute_next_grow(size) ((size) + 1)
#endif /* RVEC_LOGARITHMIC_GROWTH */

/*========================================================================*/
/*
 * Adds an element to the end of the vector.
 *
 * Parameter:
 *      vec   -> the vector
 *      value -> the value to add
 *
 * Return: void
 *
 * Checked Runtime Error:
 *      necessary allocated malloc return NULL -> Assert_Failed
 *
 * Unchecked Runtime Error: None
 * Exception: None
 *
 */
/*========================================================================*/
#define rvec_push_back(vec, value)                            \
    do {                                                      \
        size_t rv_cap = rvec_capacity(vec);                   \
        if (rv_cap <= rvec_size(vec)) {                       \
            rvec_grow((vec), rvec_compute_next_grow(rv_cap)); \
        }                                                     \
        (vec)[rvec_size(vec)] = (value);                      \
        rvec_set_size((vec), rvec_size(vec) + 1);             \
    } while (0)

/*========================================================================*/
/*
 * Insert element at position pos to the vector.
 *
 * Parameter:
 *      vec -> the vector
 *      pos -> position in the vector where the new elements are inserted
 *      val -> value to be copied (or moved) to the inserted elemtns
 *
 * Return: void
 * Checked Runtime Error: None
 * Unchecked Runtime Error: None
 * Exception: None
 *
 */
/*========================================================================*/
#define rvec_insert(vec, pos, val)                                          \
    do {                                                                    \
        if (rvec_capacity(vec) <= rvec_size(vec) + 1) {                     \
            rvec_grow((vec), rvec_compute_next_grow(rvec_capacity((vec)))); \
        }                                                                   \
        if ((pos) < rvec_size(vec)) {                                       \
            memmove((vec) + (pos) + 1, (vec) + (pos),                       \
                    sizeof(*(vec)) * ((rvec_size(vec) + 1) - (pos)));       \
        }                                                                   \
        (vec)[(pos)] = (val);                                               \
        rvec_set_size((vec), rvec_size(vec) + 1);                           \
    } while (0)

/*========================================================================*/
/*
 * Removes the last element from the vector.
 *
 * Parameter: vec -> the vector
 *
 * Return: void
 * Checked Runtime Error: None
 * Unchecked Runtime Error: None
 * Exception: None
 *
 */
/*========================================================================*/
#define rvec_pop_back(vec)                                                 \
    do {                                                                   \
        rvec_elem_destructor_t e_destructor = rvec_elem_destructor((vec)); \
        if (e_destructor) {                                                \
            e_destructor(&(vec)[rvec_size(vec) - 1]);                      \
        }                                                                  \
        rvec_set_size((vec), rvec_size(vec) - 1);                          \
    } while (0)

/*========================================================================*/
/*
 * Copy a vector
 *
 * Parameter:
 *      from -> the original vector
 *      to   -> destination to which the function copy to
 *
 * Return: void
 * Checked Runtime Error: None
 * Unchecked Runtime Error: None
 * Exception: None
 *
 */
/*========================================================================*/
#define rvec_copy(from, to)                                          \
    do {                                                             \
        if ((from)) {                                                \
            rvec_grow(to, rvec_size(from));                          \
            rvec_set_size(to, rvec_size(from));                      \
            memcpy((to), (from), rvec_size(from) * sizeof(*(from))); \
        }                                                            \
    } while (0)

/*========================================================================*/
/*
 * For internal use, sets the capacity variable of the vector.
 *
 * Parameter:
 *      vec  -> the vector
 *      size -> the new capacity to set
 *
 * Return: void
 * Checked Runtime Error: None
 * Unchecked Runtime Error: None
 * Exception: None
 *
 */
/*========================================================================*/
#define rvec_set_capacity(vec, size)        \
    do {                                    \
        if ((vec)) {                        \
            ((size_t *)(vec))[-1] = (size); \
        }                                   \
    } while (0)

/*========================================================================*/
/*
 * For internal use, sets the size variable of the vector.
 *
 * Parameter:
 *      vec -> the vector
 *      size -> the new size to set
 *
 * Return: void
 * Checked Runtime Error: None
 * Unchecked Runtime Error: None
 * Exception: None
 *
 */
/*========================================================================*/
#define rvec_set_size(vec, size)            \
    do {                                    \
        if ((vec)) {                        \
            ((size_t *)(vec))[-2] = (size); \
        }                                   \
    } while (0)

/*========================================================================*/
/*
 * For internal use, ensures that the vector is at least <count> elements
 * big.
 *
 * Parameter:
 *      vec   -> the vector
 *      count -> the new capacity to set
 *
 * Return: void
 *
 * Checked Runtime Error:
 *      malloc return NULL  -> Assert_Failed
 *      realloc return NULL -> Assert_Failed
 *
 * Unchecked Runtime Error: None
 * Exception: None
 *
 */
/*========================================================================*/
#define rvec_grow(vec, count)                                                \
    do {                                                                     \
        size_t const rv_sz = (count) * sizeof(*(vec)) + sizeof(size_t) * 2 + \
                             sizeof(rvec_elem_destructor_t);                 \
        if ((vec)) {                                                         \
            rvec_elem_destructor_t *rv_p1 =                                  \
                &((rvec_elem_destructor_t *)&((size_t *)(vec))[-2])[-1];     \
            rvec_elem_destructor_t *rv_p2 = rvec_clib_realloc(rv_p1, rv_sz); \
            rassert(rv_p2);                                                  \
            (vec) = (void *)&((size_t *)&rv_p2[1])[2];                       \
        } else {                                                             \
            rvec_elem_destructor_t *rv_p = rvec_clib_malloc(rv_sz);          \
            rassert(rv_p);                                                   \
            (vec) = (void *)&((size_t *)&rv_p[1])[2];                        \
            rvec_set_size((vec), 0);                                         \
            ((rvec_elem_destructor_t *)&(((size_t *)(vec))[-2]))[-1] = NULL; \
        }                                                                    \
        rvec_set_capacity((vec), (count));                                   \
    } while (0)

#endif /* end of include guard: RVEC_H */
