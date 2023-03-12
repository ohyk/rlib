#include "rmem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rassert.h"
#include "rexcept.h"
#include "rlog.h"

/* macro */
#define NDESCRIPTORS 512
#define HASH(p, t)   (((uintptr_t)(p) >> 3) & (sizeof(t) / sizeof((t)[0]) - 1))
#define NALLOC                                                       \
    ((4096 + sizeof(union Align_U) - 1) / (sizeof(union Align_U))) * \
        (sizeof(union Align_U))

/* checking types */
static struct Descriptor_S
{
    struct Descriptor_S *free;
    struct Descriptor_S *link;
    const void          *ptr;
    size_t               size;
    const char          *file;
    int                  line;
} *HashTable[2053];

union Align_U
{
#ifdef ALIGN_MAX
    char pad[ALIGN_MAX];
#else
    int   i;
    long  l;
    long *lp;
    void *p;
    void (*fp)(void);
    float       f;
    double      d;
    long double ld;
#endif
};

static struct Descriptor_S Freelist   = {&Freelist};
RExcept_T const            RMemFailed = {"Allocation Failed"};

static struct Descriptor_S *find(void const *ptr)
{
    struct Descriptor_S *bp = HashTable[HASH(ptr, HashTable)];
    while (bp && bp->ptr != ptr) bp = bp->link;
    return bp;
}

static struct Descriptor_S *dalloc(void *ptr, size_t size, char const *file, int line)
{
    static struct Descriptor_S *avail;
    static int                  nleft;

    if (nleft <= 0) {
        avail = malloc(sizeof(*avail) * NDESCRIPTORS);
        if (avail == NULL) return NULL;
        nleft = NDESCRIPTORS;
    }
    avail->ptr  = ptr;
    avail->file = file;
    avail->line = line;
    avail->size = size;
    avail->free = avail->link = NULL;
    nleft--;
    return avail++;
}

void rmem_free(void *ptr, char const *file, int line)
{
    if (ptr) {
        struct Descriptor_S *bp = NULL;
        if ((uintptr_t)ptr % (sizeof(union Align_U)) != 0 || (bp = find(ptr)) == NULL ||
            bp->free)
            rexcept_raise(&RAssertFailed, file, line);
        bp->free      = Freelist.free;
        Freelist.free = bp;
    }
}

void *rmem_calloc(size_t count, size_t nbytes, char const *file, int line)
{
    void *ptr;
#ifdef MEM_MAXIMUM
    rassert(count * nbytes < MEM_MAXIMUM);
#endif
    ptr = rmem_alloc(count * nbytes, file, line);
    memset(ptr, '\0', count * nbytes);
    return ptr;
}

void *rmem_resize(void *ptr, size_t nbytes, char const *file, int line)
{
    void                *newptr;
    struct Descriptor_S *bp = NULL;

    rassert(ptr);
#ifdef MEM_MAXIMUM
    rassert(nbytes < MEM_MAXIMUM);
#endif
    if ((uintptr_t)ptr % (sizeof(union Align_U)) != 0 || (bp = find(ptr)) == NULL ||
        bp->free)
        rexcept_raise(&RAssertFailed, file, line);
    newptr = rmem_alloc(nbytes, file, line);
    memcpy(newptr, ptr, nbytes < bp->size ? nbytes : bp->size);
    rmem_free(ptr, file, line);
    return newptr;
}

void *rmem_alloc(size_t nbytes, char const *file, int line)
{
    struct Descriptor_S *bp;
    void                *ptr;

#ifdef MEM_MAXIMUM
    rassert(nbytes < MEM_MAXIMUM);
#endif
    nbytes = ((nbytes + sizeof(union Align_U) - 1) / (sizeof(union Align_U))) *
             (sizeof(union Align_U));
    for (bp = Freelist.free; bp; bp = bp->free) {
        if (bp->size > nbytes) {
            bp->size -= nbytes;
            ptr = (char *)bp->ptr + bp->size;
            if ((bp = dalloc(ptr, nbytes, file, line)) != NULL) {
                unsigned h   = HASH(ptr, HashTable);
                bp->link     = HashTable[h];
                HashTable[h] = bp;
                return ptr;
            } else {
                if (file == NULL)
                    RAISE(RMemFailed);
                else
                    rexcept_raise(&RMemFailed, file, line);
            }
        }
        /* if (bp->size == sizeof (union Align_U)) { */
        /* bp->size = 0; */
        /* free((char *) bp->ptr); */
        /* } */
        if (bp == &Freelist) {
            struct Descriptor_S *newptr = NULL;
            if ((ptr = malloc(nbytes + NALLOC)) == NULL ||
                (newptr = dalloc(ptr, nbytes + NALLOC, __FILE__, __LINE__)) == NULL) {
                if (file == NULL)
                    RAISE(RMemFailed);
                else
                    rexcept_raise(&RMemFailed, file, line);
            }
            newptr->free  = Freelist.free;
            Freelist.free = newptr;
        }
    }
    rassert(0);
    return NULL;
}

void rmem_print_allocated(void)
{
    struct Descriptor_S *bp;
    int                  i;
    int                  count;

    for (i = count = 0; i < (sizeof(HashTable) / (sizeof(HashTable[0]))); i++) {
        for (bp = HashTable[i]; bp; bp = bp->link) {
            if (!bp->free) {
                rlog_message("++ allocated at %s:%d %p %zd long", bp->file, bp->line,
                             bp->ptr, bp->size);
                count++;
            }
        }
    }
    rlog_message("== allocated total: %d", count);
}

void rmem_print_freed(void)
{
    struct Descriptor_S *bp;
    bp        = Freelist.free;
    int count = 0;

    while (bp && bp != &Freelist) {
        rlog_message("-- freed %p %zd long at %s:%d", bp->ptr, bp->size, bp->file,
                     bp->line);
        count++;
        bp = bp->free;
    }
    rlog_message("== freed total: %d", count);
}

void rmem_leak(void (*apply)(void *ptr, size_t size, char const *file, int line,
                             void *cl),
               void *cl)
{
    struct Descriptor_S *bp;
    int                  i;

    for (i = 0; i < (sizeof(HashTable) / (sizeof(HashTable[0]))); i++)
        for (bp = HashTable[i]; bp; bp = bp->link)
            if (!bp->free) apply((void *)bp->ptr, bp->size, bp->file, bp->line, cl);
}

static void print_allocated_freed(void)
{
    rlog_message("[===== LEAK START =====]");
    rmem_print_allocated();
    /* rmem_print_freed(); */
    rlog_message("[===== LEAK END   =====]");
}

void rmem_atexit_leak(void) { atexit(print_allocated_freed); }
