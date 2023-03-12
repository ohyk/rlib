#include <stdio.h>
#include "mem.h"
#include "arrayptr.h"
#include "../inc/rtest.h"

int main(int argc, char *argv[])
{
    Array_T ar = arrayptr_new(10);
    int i;
    for (i = 0; i < 10; i++)
        arrayptr_put(ar, i, MAKE_INT(i));
    for (i = 0; i < 10; i++)
        printf("%d ", *(int *) arrayptr_get(ar, i));
    putchar('\n');

    printf("length: %d\n", arrayptr_length(ar));

    arrayptr_resize(ar, 15);
    printf("length: %d\n", arrayptr_length(ar));

    arrayptr_resize(ar, 5);
    printf("length: %d\n", arrayptr_length(ar));
    for (i = 0; i < arrayptr_length(ar); i++)
        printf("%d ", *(int *) arrayptr_get(ar, i));
    putchar('\n');

    Array_T ar_cpy = arrayptr_copy(ar, 10);
    printf("length: %d\n", arrayptr_length(ar_cpy));
    for (i = 0; i < arrayptr_length(ar); i++)
        printf("%d ", *(int *) arrayptr_get(ar_cpy, i));
    putchar('\n');
    return 0;
}
