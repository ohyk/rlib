#include <stdio.h>
#include "mem.h"
#include "set.h"
#include "../inc/rtest.h"

int intcmp(void const *x, void const *y)
{
    if (*(int *) x < *(int *) y) 
        return -1;
    else if (*(int *) x > *(int *) y)
        return 1;
    else 
        return 0;
}

unsigned inthash(void const *x)
{
    return *(int *)x;
}

void print(void const *x, void *cl)
{
    printf("%d ", *(int *)x);
}

void svfree(void const *x, void *cl)
{
}

int main(int argc, char *argv[])
{
    Set_T set1, set2, set3;
    set1 = set_new(0, intcmp, inthash);
    set2 = set_new(0, intcmp, inthash);
    
    int i;
    for (i = 0; i < 10; i++)
        set_put(set1, MAKE_INT(i));
    printf("set1: ");
    set_map(set1, print, NULL);
    putchar('\n');

    for (i = 5; i < 10; i++)
        set_put(set2, MAKE_INT(i));
    printf("set2: ");
    set_map(set2, print, NULL);
    putchar('\n');

    int *iptr = MAKE_INT(1204);
    if (set_member(set1, iptr))
        printf("in set\n");
    else 
        printf("not in set\n");

    printf("set length: %d (%s) [ok]\n", set_length(set1), RTEST_STATUS_STR(set_length(set1) == 10));

    set3 = set_union(set1, set2);
    printf("set3 (union): ");
    set_map(set3, print, NULL);
    putchar('\n');

    Set_T set4 = set_diff(set1, set2);
    printf("set4 (diff): ");
    set_map(set4, print, NULL);
    putchar('\n');

    Set_T set5 = set_inter(set1, set2);
    printf("set5 (inter): ");
    set_map(set5, print, NULL);
    putchar('\n');

    Set_T set6 = set_minus(set1, set2);
    printf("set6 (minus): ");
    set_map(set6, print, NULL);
    putchar('\n');


    set_map(set1, svfree, NULL);
    set_free(&set1);

    set_map(set2, svfree, NULL);
    set_free(&set2);

    set_free(&set3);
    set_free(&set4);
    set_free(&set5);
    set_free(&set6);

#ifdef MEM_CHK
    mem_print_allocated();
    mem_print_freed();
#endif

    return 0;
}
