#include <stdio.h>

#include "rlib/rarena.h"

int main(int argc, char *argv[])
{
    RArena_T a1 = rarena_new();
    printf("%p\n", a1);

    void *x = RARENA_ALLOC(a1, sizeof (int));
    x = RARENA_ALLOC(a1, 10000);
    printf("alloc 10000 bytes\n");
    rarena_print(a1);
    putchar('\n');

    printf("alloc 275 bytes\n");
    x = RARENA_ALLOC(a1, 275);
    rarena_print(a1);
    putchar('\n');

    printf("alloc 10240 bytes\n");
    x = RARENA_ALLOC(a1, 10240);
    rarena_print(a1);
    putchar('\n');

    printf("alloc 120 bytes\n");
    x = RARENA_ALLOC(a1, 120);
    rarena_print(a1);
    putchar('\n');

    printf("alloc 20 bytes\n");
    x = RARENA_ALLOC(a1, 20);
    rarena_print(a1);
    putchar('\n');
    
    printf("alloc 100 bytes\n");
    x = RARENA_ALLOC(a1, 100);
    rarena_print(a1);
    putchar('\n');

    printf("free a1\n");
    rarena_free(a1);
    rarena_print(a1);

    printf("freechunks\n");
    rarena_print_freechunks();
    return 0;
}
