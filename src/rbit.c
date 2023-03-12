#include "rbit.h"

#include <stdarg.h>
#include <string.h>

#include "rassert.h"
#include "rmem.h"

#define T RBit_T
struct T
{
    int            length;
    unsigned char *bytes;
    unsigned long *words;
};

/* macros */
#define BPW         (8 * sizeof(unsigned long))
#define NWORDS(len) ((((len) + BPW - 1) & (~(BPW - 1))) / BPW)
#define NBYTES(len) ((((len) + 8 - 1) & (~(8 - 1))) / 8)
#define SETOP(sequal, snull, tnull, op)                 \
    if (s == t) {                                       \
        rassert(s);                                     \
        return sequal;                                  \
    } else if (s == NULL) {                             \
        rassert(t);                                     \
        return snull;                                   \
    } else if (t == NULL) {                             \
        return tnull;                                   \
    } else {                                            \
        int i;                                          \
        T   set;                                        \
        rassert(s->length == t->length);                \
        set = rbit_new(s->length);                      \
        for (i = NWORDS(s->length); --i >= 0;)          \
            set->words[i] = s->words[i] op t->words[i]; \
        return set;                                     \
    }

/* static data */
unsigned char Msbmask[] = {0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80};
unsigned char Lsbmask[] = {0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};

/* static functions */
static T copy(T t)
{
    T set;
    rassert(t);
    set = rbit_new(t->length);
    if (t->length > 0) memcpy(set->bytes, t->bytes, NBYTES(t->length));
    return set;
}

/* functions */
T rbit_new(int length)
{
    T set;
    rassert(length >= 0);
    NEW(set);
    if (length > 0)
        set->words = CALLOC(NWORDS(length), sizeof(unsigned long));
    else
        set->words = NULL;
    set->bytes  = (unsigned char *)set->words;
    set->length = length;
    return set;
}

int rbit_length(T set)
{
    rassert(set);
    return set->length;
}

int rbit_count(T set)
{
    int         length  = 0, n;
    static char count[] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};

    rassert(set);
    unsigned char c;
    for (n = NBYTES(set->length); --n >= 0;) {
        c = set->bytes[n];
        length += count[c & 0xf] + count[c >> 4];
    }
    return length;
}

void rbit_free(T *set)
{
    rassert(set && *set);
    FREE((*set)->words);
    FREE(*set);
}

int rbit_get(T set, int n)
{
    rassert(set);
    rassert(n >= 0 && n < set->length);
    return ((set->bytes[n / 8] >> (n % 8)) & 1);
}

int rbit_put(T set, int n, int bit)
{
    int prev;
    rassert(set);
    rassert(bit == 0 || bit == 1);
    rassert(n >= 0 && n < set->length);
    prev = (set->bytes[n / 8] >> (n % 8) & 1);
    if (bit == 1)
        set->bytes[n / 8] |= 1 << (n % 8);
    else
        set->bytes[n / 8] &= ~(1 << (n % 8));
    return prev;
}

void rbit_clear(T set, int lo, int hi)
{
    rassert(set);
    rassert(0 <= lo && hi < set->length);
    rassert(lo <= hi);

    if (lo / 8 < hi / 8) {
        int i;
        set->bytes[lo / 8] &= ~Msbmask[lo % 8];
        for (i = lo / 8 + 1; i < hi / 8; i++) set->bytes[i] = 0;
        set->bytes[hi / 8] &= ~Lsbmask[hi % 8];
    } else {
        set->bytes[lo / 8] &= ~(Msbmask[lo % 8] & Lsbmask[hi % 8]);
    }
}

void rbit_set(T set, int lo, int hi)
{
    rassert(set);
    rassert(0 <= lo && hi < set->length);
    rassert(lo <= hi);
    if (lo / 8 < hi / 8) {
        set->bytes[lo / 8] |= Msbmask[lo % 8];
        {
            for (int i = lo / 8 + 1; i < hi / 8; i++) set->bytes[i] = 0xff;
        }
        set->bytes[hi / 8] |= Lsbmask[hi % 8];
    } else {
        set->bytes[lo / 8] |= (Msbmask[lo % 8] & Lsbmask[hi % 8]);
    }
}

void rbit_not(T set, int lo, int hi)
{
    rassert(set);
    rassert(0 <= lo && hi < set->length);
    rassert(lo <= hi);

    if (lo / 8 < hi / 8) {
        set->bytes[lo / 8] ^= Msbmask[lo % 8];
        for (int i = lo / 8 + 1; i < hi / 8; i++) set->bytes[i] ^= 0xff;
        set->bytes[hi / 8] ^= Lsbmask[hi % 8];
    } else {
        set->bytes[lo / 8] ^= (Msbmask[lo % 8] & Lsbmask[hi % 8]);
    }
}

int rbit_lt(T s, T t)
{
    int i, lt = 0;
    rassert(s && t);
    rassert(s->length == t->length);
    for (i = NWORDS(s->length); --i >= 0;) {
        if ((s->words[i] & ~t->words[i]) != 0)
            return 0;
        else if (s->words[i] != t->words[i])
            lt |= 1;
    }
    return lt;
}

int rbit_eq(T s, T t)
{
    int i;
    rassert(s && t);
    rassert(s->length == t->length);
    for (i = NWORDS(s->length); --i >= 0;)
        if (s->words[i] != t->words[i]) return 0;
    return 1;
}

int rbit_leq(T s, T t)
{
    int i;
    rassert(s && t);
    rassert(s->length == t->length);
    for (i = NWORDS(s->length); --i >= 0;)
        if (s->words[i] & ~t->words[i]) return 0;
    return 1;
}

void rbit_map(T set, void apply(int n, int bit, void *cl), void *cl)
{
    int n;
    rassert(set);
    for (n = 0; n < set->length; n++) apply(n, ((set->bytes[n / 8] >> (n % 8)) & 1), cl);
}

T rbit_union(T s, T t){SETOP(copy(t), copy(t), copy(s), |)}

T rbit_inter(T s, T t){SETOP(copy(t), rbit_new(t->length), rbit_new(s->length), &)}

T rbit_minus(T s, T t){SETOP(rbit_new(s->length), rbit_new(t->length), copy(s), &~)}

T rbit_diff(T s, T t)
{
    SETOP(rbit_new(s->length), copy(t), copy(s), ^)
}
