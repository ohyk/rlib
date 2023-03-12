#include "rmem.h"

#include <stddef.h>
#include <stdlib.h>

#include "rassert.h"
#include "rexcept.h"

/* data */
RExcept_T const RMemFailed = { "Allocation Failed" };

/* functions */
void *rmem_alloc(size_t nbytes, const char *file, int line)
{
    void *ptr;

#ifdef MEM_MAXIMUM
    rassert(nbytes < MEM_MAXIMUM);
#endif
    ptr = malloc(nbytes);
    if (ptr == NULL) {
        if (file == NULL)
            RAISE(RMemFailed);
        else 
            rexcept_raise(&RMemFailed, file, line);
    }
    return ptr;
}

void *rmem_calloc(size_t count, size_t nbytes, const char *file, int line)
{
    void *ptr;

#ifdef MEM_MAXIMUM
    rassert(count*nbytes < MEM_MAXIMUM);
#endif
    ptr = calloc(count ,nbytes);
    if (ptr == NULL) {
        if (file == NULL)
            RAISE(RMemFailed);
        else 
            rexcept_raise(&RMemFailed, file, line);
    }
    return ptr;
}

void rmem_free(void *ptr, const char *file, int line)
{
    if (ptr)
        free(ptr);
}

void *rmem_resize(void *ptr, size_t nbytes, const char *file, int line)
{
    rassert(ptr);
#ifdef MEM_MAXIMUM
    rassert(nbytes < MEM_MAXIMUM);
#endif
    ptr = realloc(ptr, nbytes);
    if (ptr == NULL) {
        if (file == NULL)
            RAISE(RMemFailed);
        else 
            rexcept_raise(&RMemFailed, file, line);
    }
    return ptr;
}
