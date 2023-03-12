#include "rvector.h"

#include <string.h>

#include "rassert.h"
#include "rlog.h"
#include "rmem.h"

#define RVECTOR_BUF_LEN 32
#define RVECTOR_FACTOR 2

#define COUNT_BYTES(v, n) ((v)->size * (n))
#define USE_LEN(v) (((v)->ap - (v)->array) / (v)->size)

#define DECREMENT_AP(v) ((v)->ap -= COUNT_BYTES((v), 1))

#define RVALLOC(v, n) ((v)->hooks->allocator(COUNT_BYTES((v), (n)), __FILE__, __LINE__))
#define RVFREE(v, p) ((void)((v)->hooks->free((p), __FILE__, __LINE__), (p) = 0))
#define RVRESIZE(v, p, n, rn)                                                         \
    do {                                                                              \
        if ((v)->hooks->resize) {                                                     \
            (p) = (v)->hooks->resize((p), COUNT_BYTES((v), (n)), __FILE__, __LINE__); \
        } else {                                                                      \
            void *newp = RVALLOC((v), (n));                                           \
            memcpy(newp, (p), COUNT_BYTES(v, (rn)));                                  \
            RVFREE((v), (p));                                                         \
            (p) = newp;                                                               \
        }                                                                             \
    } while (0)

#define T RVector_T
struct T
{
    size_t         size;     /* size of element */
    size_t         capacity; /* capacity */
    char          *array;    /* buffer */
    char          *ap;       /* next unuse element */
    RVectorHooks_T hooks;    /* allocator */
};

static struct RVectorHooks_T Default_Hooks = {rmem_alloc, rmem_free, rmem_resize};

#ifdef RVDEBUG
void rvector_print_info(T v)
{
    rlog_message("====== %p Start ======", v);
    rlog_message("size: %zd", v->size);
    rlog_message("capacity: %zd", v->capacity);
    rlog_message("ap-ar: %lld", v->ap - v->array);
    rlog_message("====== %p End   ======", v);
}
#endif

static inline int is_empty(T v) { return v->ap == v->array; }

T rvector_new(size_t size, RVectorHooks_T hk)
{
#ifdef MEM_MAXIMUM
    rassert(size < MEM_MAXIMUM);
#else
    rassert(size < 32767);
#endif
    T v = NULL;
    if (hk) {
        rassert(hk->allocator);
        rassert(hk->free);
        v        = hk->allocator(sizeof(*v), __FILE__, __LINE__);
        v->hooks = hk;
    } else {
        v        = Default_Hooks.allocator(sizeof(*v), __FILE__, __LINE__);
        v->hooks = &Default_Hooks;
    }

    v->size     = size;
    v->capacity = RVECTOR_BUF_LEN;

    v->array = v->ap = RVALLOC(v, v->capacity);
    return v;
}

void rvector_reset(T v)
{
    rassert(v);
    v->ap = v->array;
}

void rvector_append(T v, void const *x)
{
    rassert(v);
    rassert(x);
    if (USE_LEN(v) == v->capacity) {
        /* array need expand */
        RVRESIZE(v, v->array, v->capacity * RVECTOR_FACTOR, v->capacity);
        v->ap = v->array + COUNT_BYTES(v, v->capacity);
        v->capacity *= RVECTOR_FACTOR;
    }
    for (int i = 0; i < v->size; ++i, ++(v->ap), ++x) *(char *)v->ap = *(char *)x;
}

void *rvector_subtract(T v)
{
    rassert(v);
    void *res = NULL;
    if (!is_empty(v)) {
        DECREMENT_AP(v);
        res = v->ap;
    }
    return res;
}

void *rvector_get(T v)
{
    rassert(v);
    return v->array;
}

void *rvector_get_dispose(T *vp)
{
    rassert(vp);
    rassert(*vp);
    void *res = NULL;
    if (!is_empty(*vp)) {
        res = (*vp)->array;
    } else {
        RVFREE(*vp, (*vp)->array);
    }
    RVFREE(*vp, *vp);
    return res;
}

void *rvector_get_dispose_shrink(T *vp)
{
    rassert(vp);
    rassert(*vp);
    void *res = NULL;
    if (!is_empty(*vp)) {
        RVRESIZE(*vp, (*vp)->array, USE_LEN(*vp), USE_LEN(*vp));
        res = (*vp)->array;
    } else {
        RVFREE(*vp, (*vp)->array);
    }
    RVFREE(*vp, *vp);
    return res;
}

size_t rvector_length(T v)
{
    rassert(v);
    return is_empty(v) ? 0 : USE_LEN(v);
}

size_t rvector_size(T v)
{
    rassert(v);
    return v->size;
}

void rvector_reverse(T v, size_t n_elem)
{
    rassert(v);
    rassert(n_elem > 0);
    if (v->capacity < n_elem) {
        size_t offt = v->ap - v->array;
        RVRESIZE(v, v->array, n_elem, USE_LEN(v));
        v->ap       = v->array + offt;
        v->capacity = n_elem;
    }
}

size_t rvector_capacity(T v)
{
    rassert(v);
    return v->capacity;
}
