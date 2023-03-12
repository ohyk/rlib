#include "rarena.h"

#include <stdlib.h>
#include <string.h>

#include "rassert.h"
#include "rexcept.h"

#define T RArena_T

const RExcept_T RArenaNewFailed = {"RArena Creation Failed"};
const RExcept_T RArenaFailed    = {"RArena Allocation Failed"};

/* macros */
#define THRESHOLD 10

/* types */
struct T
{
    T     prev;
    char *avail;
    char *limit;
};

union align
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

union Header_U
{
    struct T    b;
    union align a;
};

/* data */
static T FreeChunks;
/* static T freemaxchunk; */
static int NFree;

/* functions */
T rarena_new(void)
{
    T arena = malloc(sizeof(*arena));
    if (arena == NULL) RAISE(RArenaNewFailed);
    arena->prev  = NULL;
    arena->avail = arena->limit = NULL;
    return arena;
}

void rarena_dispose(T *ap)
{
    rassert(ap && *ap);
    rarena_free(*ap);
    free(*ap);
    *ap = NULL;
}

void *rarena_alloc(T arena, size_t nbytes, const char *file, int line)
{
    rassert(arena);
#ifdef MEM_MAXIMUM
    rassert(nbytes < MEM_MAXIMUM);
#endif
    /* round nbytes up to alignment boundary */
    nbytes = ((nbytes + sizeof(union align) - 1) / (sizeof(union align))) *
             (sizeof(union align));

    /* if allocated previous chunks has nbytes */
    if (nbytes > arena->limit - arena->avail) {
        T ptr = arena ? arena->prev : NULL;
        while (ptr) {
            if (nbytes < ptr->limit - ptr->avail) {
                ptr->avail += nbytes;
                return ptr->avail - nbytes;
            }
            ptr = ptr->prev;
        }
    }

    /* check free chunks or get new chunk */
    while (nbytes > arena->limit - arena->avail) {
        T     ptr;
        char *limit;
        /* ptr <- a new chunk */
        if ((ptr = FreeChunks) != NULL) {
            FreeChunks = FreeChunks->prev;
            NFree--;
            limit = ptr->limit;
        } else {
            /* get a new chunk */
            size_t m = sizeof(union Header_U) + nbytes + 10 * 1024;
            ptr      = malloc(m);
            if (ptr == NULL) {
                if (file == NULL)
                    RAISE(RArenaFailed);
                else
                    rexcept_raise(&RArenaFailed, file, line);
            }
            limit = (char *)ptr + m;
        }
        *ptr         = *arena;
        arena->avail = (char *)((union Header_U *)ptr + 1);
        arena->limit = limit;
        arena->prev  = ptr;
    }
    arena->avail += nbytes;
    return arena->avail - nbytes;
}

void *rarena_calloc(T arena, size_t count, size_t nbytes, const char *file, int line)
{
    void *ptr;

#ifdef MEM_MAXIMUM
    rassert(count * nbytes < MEM_MAXIMUM);
#endif
    ptr = rarena_alloc(arena, count * nbytes, file, line);
    memset(ptr, '\0', count * nbytes);
    return ptr;
}

void rarena_free(T arena)
{
    rassert(arena);
    while (arena->prev) {
        struct T tmp = *arena->prev;
        /* free the chunk described by arena */
        if (NFree < THRESHOLD) {
            arena->prev->prev = FreeChunks;
            FreeChunks        = arena->prev;
            FreeChunks->limit = arena->limit;
            NFree++;
        } else
            free(arena->prev);
        *arena = tmp;
    }
    rassert(arena->limit == NULL);
    rassert(arena->avail == NULL);
}

#ifdef ARENA_DEBUG
#include "rlog.h"
void rarena_print(T arena)
{
    rassert(arena);

    for (; arena; arena = arena->prev) {
        rlog_message("--- %p ---", arena);
        rlog_message("prev: %p", arena->prev);
        rlog_message("avail: %p", arena->avail);
        rlog_message("limit: %p", arena->limit);
        rlog_message("size: %llu", arena->limit - arena->avail);
        rlog_message("--- end ---");
    }
}

void rarena_print_freechunks(void)
{
    rlog_message("=== free chunks ===");
    if (FreeChunks) {
        rarena_print(FreeChunks);
    }
}
#endif
