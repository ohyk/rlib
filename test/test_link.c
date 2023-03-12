#include <stdio.h>
#include "mem.h"
#include "link.h"
#include "../inc/rtest.h"

void del(void *x)
{
    /* FREE(x); */
}

void apply_print(void **x, void *cl)
{
    printf("%d ", **(int **) x);
}

int main(int argc, char *argv[])
{
    int i;
    void *x;
    void **ar;
    Link_T l = link_create();
    Link_T l2;

    link_append(l, MAKE_INT(8989));
    link_insert(l, MAKE_INT(432), 1);
    link_map(l, apply_print, NULL);
    putchar('\n');
    link_make_empty(l, del);
    /* link_pop(l, &x); */
    /* FREE(x); */


    printf("link is empty: %s (ok)\n", RTEST_STATUS_STR(link_is_empty(l)));
    for (i = 0; i < 5; i++) 
        link_push(l, MAKE_INT(i));
    /* link_map(l, apply_print, NULL); */
    /* putchar('\n'); */

    /* if (link_find_kth(l, 1)) { */
        /* printf("\n\n%d\n\n", *(int *)link_find_kth(l, 1)); */
    /* } else { */
        /* return 123; */
    /* } */
    /* if (link_find_kth(l, link_size(l))) { */
        /* printf("\n\n%d\n\n", *(int *)link_find_kth(l, link_size(l))); */
    /* } else { */
        /* return 123; */
    /* } */
    for (i = 1; i <= 5; i++)
        printf("%d ", *(int *) link_find_kth(l, i));
    putchar('\n');

    l2 = link_copy(l);
    printf("link copy: %s (ok)\n", RTEST_STATUS_STR(l2 != l));
    link_append(l2, MAKE_INT(123));
    printf("** link append: %s (ok)\n", 
            RTEST_STATUS_STR(*(int *)(link_find_kth(l2, link_size(l2))) == 123));
    printf("---- %d\n", link_size(l2));

    printf("link to array: ");
    ar = link_to_array(l2, NULL);
    for (i = 0; i < link_size(l2); i++)
        printf("%d ", *((int *)ar[i]));
    putchar('\n');
    /* FREE(ar); */

    link_reverse(l2);
    link_map(l2, apply_print, NULL);
    putchar('\n');
    link_pop(l2, &x);
    link_map(l2, apply_print, NULL);
    putchar('\n');
    printf("x: %d\n", *(int *)x);
    /* FREE(x); */
    link_reverse(l2);
    link_map(l2, apply_print, NULL);
    putchar('\n');
    link_push(l2, MAKE_INT(99));
    link_map(l2, apply_print, NULL);
    putchar('\n');

    link_delete_kth(l2, del, 1);
    printf("delete: %s (ok)\n", RTEST_STATUS_STR(*(int *) link_find_kth(l, 1) == 4));
    link_map(l, apply_print, NULL);
    putchar('\n');

    printf("link size: %s (ok) (5) [%d]\n", RTEST_STATUS_STR(link_size(l2) == 5), link_size(l2));

    printf("link is empty: %s (error)\n", RTEST_STATUS_STR(link_is_empty(l)));

    link_make_empty(l, NULL);
    printf("make empty success: %s (ok)\n", RTEST_STATUS_STR(link_is_empty(l)));
    printf("link size: %s (ok) (0) [%d]\n", RTEST_STATUS_STR(link_size(l) == 0), link_size(l));
    link_make_empty(l2, del);
    printf("make empty success: %s (ok)\n", RTEST_STATUS_STR(link_is_empty(l2)));
    printf("link size: %s (ok) (0) [%d]\n", RTEST_STATUS_STR(link_size(l2) == 0), link_size(l2));
    FREE(l);
    FREE(l2);

#ifdef MEM_CHK
    mem_print_allocated();
    mem_print_freed();
#endif
    
    return 0;
}
