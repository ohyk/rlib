#include <stdio.h>
#include "automem.h"

int main(int argc, char *argv[])
{
    int i;
    int *par[10] = { NULL };

    for (i = 0; i < 10; i++) 
        printf("%p ", par[i]);
    putchar('\n');

    for (i = 0; i < 10; i++) {
        par[i] = AUTOMEM_ALLOC(sizeof (int));
    }

    for (i = 0; i < 10; i++) 
        printf("%p ", par[i]);
    putchar('\n');
    return 0;
}
