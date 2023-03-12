#include <stdio.h>
#include "arena.h"
#include "../inc/rtest.h"

static void print_str_hex(char const *str, int len)
{
    int i;
    for (i = 0; i < len; i++)
        printf("%x ", str[i]);
    putchar('\n');
}

int main(int argc, char *argv[])
{
    int *iptr = NEW_INT(10);
    char *alphas = MAKE_RANDOM_STR_ALPHA(10);
    char *str = NEW_STR("fuck");
    int *nums = MAKE_RAND_INTS(20);
    int *runi = MAKE_UNIFORM_INTS(50, 10);
    int *nnums = MAKE_NOT_REPEAT_INTS(50);
    int i;

    printf("%d\n", *iptr);

    for (i = 0; i < 10; i++)
        putchar(alphas[i]);
    putchar('\n');

    printf("%s\n", str);
    
    for (i = 0; i < 20; i++)
        printf("%d ", nums[i]);
    putchar('\n');

    for (i = 0; i < 50; i++)
        printf("%d ", runi[i]);
    putchar('\n');

    for (i = 0; i < 50; i++)
        printf("%d ", nnums[i]);
    putchar('\n');

    printf("uniform: \n");
    for (i = 0; i < 10; i++)
        printf("%d\n", rtest_random_int(10));

    /* make range */
    int len, s, e, st;
    s = 10, e = 20, st = 1;
    int *r1 = MAKE_RANGE1(s, e);
    /* r1: 10, 11, 12, 13, 14, 15...19 */
    printf("range: ");
    for (i = 0, len = ((e-s)/st); i < len; i++)
        printf("%d ", r1[i]);
    putchar('\n');

    /* r1: -10, -8, -6...18 */
    s = -10, e = 20, st = 2;
    r1 = MAKE_RANGE(s, e, st);
    printf("range: ");
    for (i = 0, len = ((e-s)/st); i < len; i++)
        printf("%d ", r1[i]);
    putchar('\n');

    s = 1, e = 20, st = 19;
    r1 = MAKE_RANGE(s, e, st);
    printf("range: ");
    for (i = 0, len = ((e-s)/st); i < len; i++)
        printf("%d ", r1[i]);
    putchar('\n');

    /* step <= 0 --> Assert_Failed */
    /* s = 1, e = 20, st = -1; */
    /* r1 = MAKE_RANGE(s, e, st); */

    /* s > e --> Assert_Failed */
    /* s = 20, e = 10, st = 1; */
    /* r1 = MAKE_RANGE(s, e, st); */

    /* (e-s)/step == 0 --> Assert_Failed */
    /* s = 1, e = 10, st = 10; */
    /* r1 = MAKE_RANGE(s, e, st); */

    char buf[11];
    int buflen = rtest_make_random_str_with_str(buf, 5, Rtest_Make_Str_Mode_Word);
    printf("%s - %d\n", buf, buflen);
    print_str_hex(buf, 11);


#ifdef ARENA_DEBUG
    rtest_mem_print();
#endif
    rtest_mem_dispose();
    return 0;
}
