#include "rexcept.h"

#include <stdio.h>
#include <stdlib.h>

#include "rassert.h"

#ifdef RLIB_HAS_EXECINFO
#include <execinfo.h> /* call stack backtrace */
#endif

#define T RExcept_T

RExceptFrame_T *RExceptStack = NULL;

void rexcept_raise(T const *e, char const *file, int line)
{
    RExceptFrame_T *p = RExceptStack;

    rassert(e);

    if (p == NULL) {
        fprintf(stderr, "Uncaught exception ");
        if (e->reson)
            fprintf(stderr, "%s", e->reson);
        else 
            fprintf(stderr, "at 0x%p", e);
        if (file && line > 0)
            fprintf(stderr, " raised at %s:%d\n", file, line);
#ifdef RLIB_HAS_EXECINFO
        void *call_stack[64];
        int i, size = backtrace(call_stack, 64);
        char **symbols = backtrace_symbols(call_stack, size);
        if (!symbols) {
            perror("rexcept_raise backtrace_symbols: ");
        } else {
            fprintf(stderr, "-- backtrace --\n");
            for (i = 0; i < size; ++i)
                fprintf(stderr, "%s\n", symbols[i]);
            free(symbols);
        }
#endif
        fprintf(stderr, "aborting...\n");
        fflush(stderr);
        abort();
    }
    p->exception = e;
    p->file = file;
    /* p->func = func; */
    p->line = line;
    RExceptStack = RExceptStack->prev;
    longjmp(p->env, RExceptRaised);
}

