#include <stdio.h>

#include "../inc/rtest.h"
#include "assert.h"
#include "mem.h"
#include "ring.h"

static void ring_map(Ring_T ring, void app(void *x, void *cl), void *cl)
{
    int i;
    assert(ring);
    for (i = 0; i < ring_length(ring); i++) app(ring_get(ring, i), cl);
}

static void app_print_int(void *x, void *cl) { printf("%d ", *(int *)x); }

int main(int argc, char *argv[])
{
    Ring_T ring = ring_new();
    int    i;
    int *  iptr;

    /* [ ] */
    printf("empty ring: %s [ok] (%d)\n", RTEST_STATUS_STR(ring_length(ring) == 0),
           ring_length(ring));

    for (i = 0; i < 5; i++) ring_addhi(ring, NEW_INT(i));

    /* [ 0 1 2 3 4 ] */
    printf("ring have 5 entry: %s [ok] (%d)\n", RTEST_STATUS_STR(ring_length(ring) == 5),
           ring_length(ring));

    ring_map(ring, app_print_int, NULL);
    putchar('\n');

    /* [ (99) 0 1 2 3 4 ] */
    /*   ^  */
    ring_addlo(ring, NEW_INT(99));
    printf("ring addlo: %s [ok] (%d)\n",
           RTEST_STATUS_STR(*(int *)ring_get(ring, 0) == 99), *(int *)ring_get(ring, 0));

    ring_map(ring, app_print_int, NULL);
    putchar('\n');

    /* [ 99 0 1 2 3 4 ] */
    /*   <--    */
    ring_rotate(ring, 1);
    /* [ 0 1 2 3 4 99 ] */
    printf("ring rotate: %s [ok] (%d)\n",
           RTEST_STATUS_STR(*(int *)ring_get(ring, 0) == 0), *(int *)ring_get(ring, 0));

    ring_map(ring, app_print_int, NULL);
    putchar('\n');

    /* [ 0 1 2 3 4 99 ] */
    /*   -->    */
    ring_rotate(ring, -1);
    /* [ 99 0 1 2 3 4 ] */
    printf("ring rotate: %s [ok] (%d)\n",
           RTEST_STATUS_STR(*(int *)ring_get(ring, 0) == 99), *(int *)ring_get(ring, 0));

    ring_map(ring, app_print_int, NULL);
    putchar('\n');

    iptr = ring_remlo(ring);
    printf("ring remove low: %s [ok] (%d)\n", RTEST_STATUS_STR(ring_length(ring) == 5),
           ring_length(ring));
    printf("ring remove low return: %d\n", *iptr);

    iptr = ring_remhi(ring);
    printf("ring remove high: %s [ok] (%d)\n", RTEST_STATUS_STR(ring_length(ring) == 4),
           ring_length(ring));
    printf("ring remove high return: %d\n", *iptr);

    iptr = ring_put(ring, 3, iptr);
    printf("ring put 4 in 3: %s [ok] (%d)\n",
           RTEST_STATUS_STR(*(int *)ring_get(ring, 3) == 4), *(int *)ring_get(ring, 3));
    printf("ring put return: %d\n", *iptr);

    ring_map(ring, app_print_int, NULL);
    putchar('\n');

    ring_free(&ring);

#ifdef MEM_CHK
    printf("\n=== MEM ===\n");
    mem_print_allocated();
    mem_print_freed();
#endif
    return 0;
}
