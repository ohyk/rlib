#include <stdio.h>
#include "mem.h"
#include "seq.h"
#include "../inc/rtest.h"

void seq_print(Seq_T seq, void print(void *, void *), void *cl)
{
    int n = seq_length(seq);
    int i;
    printf("P [ ");
    for (i = 0; i < n; i++)
        print(seq_get(seq, i), cl);
    printf("]");
    putchar('\n');
    fflush(stdout);
}

void seq_map_vfree(Seq_T seq)
{
    int *iptr;
    do {
        iptr = seq_remhi(seq);
    } while (seq_length(seq) > 0);
    /* for ( ; seq_length(seq) > 0; iptr = seq_remhi(seq)) */
        /* FREE(iptr); */
}

void seq_print_intp(void *x, void *cl)
{
    printf("%d ", *(int *) x);
}

void seq_print_strp(void *x, void *cl)
{
    printf("%s ", x);
}

int main(int argc, char *argv[])
{
    int i;
    int *iptr;
    Seq_T seq = seq_new(0);

    printf("seq length: %d\n", seq_length(seq));
    for (i = 0; i < 5; i++)
        seq_addhi(seq, MAKE_INT(i));

    for (i = 5; i < 10; i++)
        seq_addlo(seq, MAKE_INT(i));

    printf("seq length: %d\n", seq_length(seq));

    printf("C [ 9 8 7 6 5 0 1 2 3 4 ]\n");
    seq_print(seq, seq_print_intp, NULL);

    printf("seq length : %s (ok) [10]\n", RTEST_STATUS_STR(seq_length(seq) == 10));

    iptr = seq_put(seq, 0, MAKE_INT(99));
    printf("prev: %d\n", *iptr);
    printf("C [ 99 8 7 6 5 0 1 2 3 4 ]\n");
    seq_print(seq, seq_print_intp, NULL);

    for (i = 10; i < 20; i++)
        seq_addhi(seq, MAKE_INT(i));
    printf("seq length : %s (ok) [20]\n", RTEST_STATUS_STR(seq_length(seq) == 20));
    printf("C [ 99 8 7 6 5 0 1 2 3 4 10 11 12 13 14 15 16 17 18 19 ]\n");
    seq_print(seq, seq_print_intp, NULL);

    iptr = seq_remlo(seq);
    printf("remlo prev: %s (ok) [%d]\n", RTEST_STATUS_STR(*iptr == 99), *iptr);
    printf("seq rem befor length : %s (ok) [19]\n", RTEST_STATUS_STR(seq_length(seq) == 19));

    iptr = seq_remhi(seq);
    printf("remhi prev: %s (ok) [%d]\n", RTEST_STATUS_STR(*iptr == 19), *iptr);
    printf("seq rem befor length : %s (ok) [18]\n", RTEST_STATUS_STR(seq_length(seq) == 18));

    seq_map_vfree(seq);
    seq_free(&seq);
    printf("seq free: %s [ok]\n", RTEST_STATUS_STR(!seq));
#ifdef MEM_CHK
    mem_print_allocated();
    mem_print_freed();
#endif

    Seq_T seq2 = seq_seq("a", "b", "c", "d", NULL);
    printf("C [ a b c d ]\n");
    seq_print(seq2, seq_print_strp, NULL);
    
    Seq_T seq_empty = seq_seq(NULL);
    printf("seq_empty length: %d\n", seq_length(seq_empty));
    printf("seq_empty length : %s (ok) [0]\n", RTEST_STATUS_STR(seq_length(seq_empty) == 0));
    return 0;
}
