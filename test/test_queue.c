#include <stdio.h>

#include "queue.h"
#include "mem.h"
#include "../inc/rtest.h"

int main(int argc, char *argv[])
{
    Queue_T q = queue_new();
    int i;
    int *ip;

    printf("queue is empty? %s (ok)\n", RTEST_STATUS_STR(queue_is_empty(q)));
    printf("Enqueue: ");
    for (i = 0; i < 5; i++) {
        printf("%d ", i);
        queue_enqueue(MAKE_INT(i), q);
    }
    putchar('\n');
    printf("queue is empty? %s (err)\n", RTEST_STATUS_STR(queue_is_empty(q)));
#ifdef DEBUG_MEMORY
    fprintf(stderr, "gets: %d\tfree: %d\tall: %d\n", 
            get_rmalloc_num(), get_rfree_num(), get_rmalloc_num()-get_rfree_num());
#endif

    printf("Dequeue: ");
    for (i = 0; i < 5; i++) {
        ip = (int *) queue_dequeue(q);
        printf("%d ", *ip);
    }
    putchar('\n');
    printf("queue is empty? %s (ok)\n", RTEST_STATUS_STR(queue_is_empty(q)));

    printf("empty dequeue (equal NULL): %s (ok)\n", RTEST_STATUS_STR(queue_dequeue(q) == NULL));

    printf("Dequeu: ");
    for (i = 0; i < 5; i++) {
        printf("%d ", i);
        queue_enqueue(MAKE_INT(i), q);
    }
    putchar('\n');
    printf("queue is empty? %s (err)\n", RTEST_STATUS_STR(queue_is_empty(q)));

    queue_free(&q);
    printf("destroy queue: %s (ok)\n", RTEST_STATUS_STR(!q));

#ifdef MEM_CHK
    mem_print_allocated();
    mem_print_freed();
#endif

#ifdef DEBUG_MEMORY
    fprintf(stderr, "gets: %d\tfree: %d\tall: %d\n", 
            get_rmalloc_num(), get_rfree_num(), get_rmalloc_num()-get_rfree_num());
    fprintf(stderr, "mem all? %s (ok)\n", RTEST_STATUS_STR(get_rmalloc_num()-get_rfree_num() == 0));
#endif
    return 0;
}
