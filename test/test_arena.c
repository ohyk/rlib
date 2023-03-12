#define ARENA_DEBUG

#include <stdio.h>

#include "rarena.h"
#include "rtest.h"


int main(int argc, char *argv[])
{
    unsigned long acount = 0;
    int n;
    void *np;
    RArena_T arena = rarena_new();
    rarena_print(arena);
    RARENA_ALLOC(arena, 10);
    acount += 10;
    rarena_print(arena);
    printf("\n\n");

    for (int i = 0; i < 10; i++) {
        n = rtest_random_int(10240);
        RARENA_NEW(arena, np);
        printf("\t\t%p\n", np);
        rarena_print(arena);
        acount += n;
        printf("++ current: %d - all: %lu\n", n, acount);
    }

    rarena_free(arena);
    rarena_print(arena);

    return 0;
}
