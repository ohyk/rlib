#include "rseq.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "rassert.h"
#include "rmem.h"

#define T RSeq_T

struct T
{
    struct Node_S
    {
        struct Node_S *rlink, *llink;
        void        *x;
    }  *head;
    int length;
};

T rseq_new(int hint)
{
    rassert(hint >= 0);
    T            seq;
    struct Node_S *p, *q;
    int          i;
    if (hint == 0) hint = 16;
    NEW0(seq);
    NEW(seq->head);
    seq->head->rlink = seq->head->llink = seq->head;
    seq->head->x                        = NULL;
    q                                   = seq->head;
    for (i = 1; i < hint; i++) {
        NEW(p);
        p->rlink        = q;
        p->llink        = q->llink;
        q->llink->rlink = p;
        q->llink        = q;
        q               = p;
    }
    seq->length = hint;
    return seq;
}

T rseq_seq(void *x, ...)
{
    va_list ap;
    T       seq = rseq_new(0);

    va_start(ap, x);
    for (; x; x = va_arg(ap, void *)) rseq_addhi(seq, x);
    va_end(ap);
    return seq;
}

void rseq_free(T *seq)
{
    rassert(seq && *seq);
    if ((*seq)->length > 0) {
        struct Node_S *p, *q;
    }
    FREE(*seq);
}

extern int rseq_length(T seq);

extern void *rseq_get(T seq, int i);
extern void *rseq_put(T seq, int i, void *x);

extern void *rseq_addlo(T seq, void *x);
extern void *rseq_addhi(T seq, void *x);

extern void *rseq_remlo(T seq);
extern void *rseq_remhi(T seq);
