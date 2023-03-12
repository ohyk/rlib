#include "rseq.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "rarray.h"
#include "rarrayrep.h"
#include "rassert.h"
#include "rmem.h"

#define SEQ_I(seq, i) ((void **)(seq)->array[(((seq)->head + (i)) % (seq)->array.length)])

#define T RSeq_T

struct T
{
    struct RArray_T array;
    int             length;
    int             head;
};

/* static functions */
static void grow(T seq)
{
    int n = seq->array.length;
    rarray_resize(&seq->array, 2 * n);
    if (seq->head > 0) {
        void **old = &((void **)seq->array.array)[seq->head];
        memcpy(old + n, old, (n - seq->head) * sizeof(void *));
        seq->head += n;
    }
}

/* functions */
T rseq_new(int hint)
{
    T seq;

    rassert(hint >= 0);

    NEW0(seq);
    if (hint == 0) hint = 16;
    rarrayrep_init(&seq->array, hint, sizeof(void *), ALLOC(hint * sizeof(void *)));
    return seq;
}

T rseq_seq(void *x, ...)
{
    T       seq = rseq_new(0);
    va_list ap;

    va_start(ap, x);
    for (; x; x = va_arg(ap, void *)) rseq_addhi(seq, x);
    va_end(ap);
    return seq;
}

void rseq_free(T *seq)
{
    rassert(seq && *seq);
    rassert((void *)*seq == (void *)&(*seq)->array);
    rarray_free((RArray_T *)seq);
}

int rseq_length(T seq)
{
    rassert(seq);
    return seq->length;
}

void *rseq_get(T seq, int i)
{
    rassert(seq);
    rassert(i >= 0 && i < seq->length);
    return ((void **)seq->array.array)[(seq->head + i) % seq->array.length];
}

void *rseq_put(T seq, int i, void *x)
{
    void *prev;

    rassert(seq);
    rassert(i >= 0 && i < seq->length);

    prev = ((void **)seq->array.array)[(seq->head + i) % seq->array.length];
    ((void **)seq->array.array)[(seq->head + i) % seq->array.length] = x;
    return prev;
}

void *rseq_remlo(T seq)
{
    int   i = 0;
    void *x;

    rassert(seq);
    rassert(seq->length > 0);

    x         = ((void **)seq->array.array)[(seq->head + i) % seq->array.length];
    seq->head = (seq->head + 1) % seq->array.length;
    --seq->length;
    return x;
}

void *rseq_remhi(T seq)
{
    int i;

    rassert(seq);
    rassert(seq->length > 0);

    i = --seq->length;
    return ((void **)seq->array.array)[(seq->head + i) % seq->array.length];
}

void *rseq_addlo(T seq, void *x)
{
    int i = 0;

    rassert(seq);

    if (seq->length == seq->array.length) grow(seq);
    if (--seq->head < 0) seq->head = seq->array.length - 1;
    seq->length++;
    return ((void **)seq->array.array)[(seq->head + i) % seq->array.length] = x;
}

void *rseq_addhi(T seq, void *x)
{
    int i;

    rassert(seq);

    if (seq->length == seq->array.length) grow(seq);
    i = seq->length++;
    return ((void **)seq->array.array)[(seq->head + i) % seq->array.length] = x;
}
