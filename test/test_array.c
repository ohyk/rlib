#include <stdio.h>
#include <limits.h>
#include "mem.h"
#include "array.h"

int main(int argc, char *argv[])
{
    Array_T ar = array_new(10, sizeof (int));
    Array_T ar_cpy;
    int i;

    printf("%d\n", array_length(ar));
    printf("%d\n", array_size(ar));
    
    for (i = 0; i < 10; i++)
        array_put(ar, i, &i);
    for (i = 0; i < 10; i++)
        printf("%d ", *(int *) array_get(ar, i));
    putchar('\n');

    array_resize(ar, 15);
    for (i = 10; i < 15; i++)
        array_put(ar, i, &i);
    for (i = 0; i < 15; i++)
        printf("%d ", *(int *) array_get(ar, i));
    putchar('\n');

    ar_cpy = array_copy(ar, 10);
    for (i = 0; i < array_length(ar_cpy); i++)
        printf("%d ", *(int *) array_get(ar_cpy, i));
    putchar('\n');


    printf("%x\n", UINT_MAX);
    unsigned imax = UINT_MAX;
    for (i = 0; i < array_length(ar); i++)
        array_put(ar, i, &imax);
    for (i = 0; i < array_length(ar); i++)
        printf("%x ", *(int *) array_get(ar, i));
    putchar('\n');

    array_reshape(ar, 10, sizeof (unsigned long));

    printf("length: %d\n", array_length(ar));
    printf("size: %d\n", array_size(ar));
    printf("%ld\n", sizeof (unsigned long));
    for (i = 0; i < array_length(ar); i++)
        printf("%x ", *(int *) array_get(ar, i));
    putchar('\n');

    printf("ULONG_MAX: %lx\n", ULONG_MAX);
    unsigned long lmax = ULONG_MAX;
    for (i = 0; i < array_length(ar); i++)
        array_put(ar, i, &lmax);
    for (i = 0; i < array_length(ar); i++)
        printf("%lx ", *(unsigned long *) array_get(ar, i));
    putchar('\n');

    array_reshape(ar, 15, sizeof (unsigned));
    for (i = 0; i < array_length(ar); i++)
        printf("%x ", *(int *) array_get(ar, i));
    putchar('\n');

    
    array_free(&ar);
    array_free(&ar_cpy);

#ifdef MEM_CHK
    mem_print_allocated();
    mem_print_freed();
#endif

    return 0;
}
