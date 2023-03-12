#include <stdio.h>
#include "mem.h"
#include "expand.h"

void print_iar(int *iar, int len)
{
    for (int i = 0; i < len; ++i)
        printf("%d ", iar[i]);
    putchar('\n');
}

int main(int argc, char *argv[])
{
    int *iar = NULL;
    int i;
    Expand_T buf = expand_new(sizeof (int));

    for (i = 0; i < 233; i++)
        expand_append(buf, &i);

    iar = expand_get(buf);

    printf("iar(%p): \n", iar);
    for (i = 0; i < 233; i++)
        printf("%d ", iar[i]);
    putchar('\n');

    iar = expand_get_dispose(&buf);

    printf("iar(%p): \n", iar);
    for (i = 0; i < 233; i++)
        printf("%d ", iar[i]);
    putchar('\n');
    
    printf("buf: %p\n", buf);

    FREE(iar);
    printf("iar: %p\n", iar);

#ifdef MEM_CHK
    mem_print_allocated();
    mem_print_freed();
#endif

    printf("---\n\n");

    long *lar = NULL;
    long l;
    buf = expand_new(sizeof (long));

    for (l = 0; l < 333; l++)
        expand_append(buf, &l);

    lar = expand_get(buf);

    printf("lar(%p) use len(%ld): \n", lar, expand_use_length(buf));
    for (i = 0; i < 333; i++)
        printf("%ld ", lar[i]);
    putchar('\n');

    iar = expand_get_dispose(&buf);

    printf("lar(%p): \n", lar);
    for (i = 0; i < 333; i++)
        printf("%ld ", lar[i]);
    putchar('\n');
    
    printf("buf: %p\n", buf);

    FREE(iar);
    printf("iar: %p\n", iar);

#ifdef MEM_CHK
    mem_print_allocated();
    mem_print_freed();
#endif

    printf("---\n\n");

    buf = expand_new(sizeof (*iar));
    for (i = 0; i < 220; i++)
        expand_append(buf, &i);
    
    iar = expand_get(buf);
    printf("iar(%p) use len(%ld): \n", iar, expand_use_length(buf));
    for (i = 0; i < 220; i++)
        printf("%d ", iar[i]);
    putchar('\n');

    printf("subtract: %d\n", *(int *) expand_subtract(buf));
    long ilen = expand_use_length(buf);
    printf("subtract after use length: %ld\n", ilen);

    printf("subtract: %d\n", *(int *) expand_subtract(buf));
    ilen = expand_use_length(buf);
    printf("subtract after use length: %ld\n", ilen);

    for (ilen = expand_use_length(buf); ilen > 0; ilen = expand_use_length(buf))
        printf("before length: %ld --- subtract: %d\n", ilen, *(int *) expand_subtract(buf));

    iar = expand_get_dispose(&buf);

    printf("iar(%p): \n", iar);
    if (ilen > 0) {
        for (i = 0; i < ilen; i++)
            printf("%d ", iar[i]);
        putchar('\n');
    }

    FREE(iar);
    printf("iar: %p\n", iar);

#ifdef MEM_CHK
    mem_print_allocated();
    mem_print_freed();
#endif

    printf("\n=== test expand_reset ===\n");
    buf = expand_new(sizeof (int));

    printf("add 10 element to buf...\n");
    for (i = 0; i < 10; ++i)
        expand_append(buf, &i);
    iar = expand_get(buf);
    printf("buf vals: ");
    print_iar(iar, 10);
    printf("buf use length: %ld\n", expand_use_length(buf));
    printf("buf capicaty: %ld\n", expand_capacity(buf)/expand_size(buf));

    printf("buf reset...\n");
    expand_reset(buf);
    printf("buf use length: %ld\n", expand_use_length(buf));
    printf("buf capicaty: %ld\n", expand_capacity(buf)/expand_size(buf));

    i = 999;
    expand_append(buf, &i);

    printf("buf vals: ");
    print_iar(iar, 10);

#ifdef MEM_CHK
    mem_print_allocated();
    mem_print_freed();
#endif

    return 0;
}
