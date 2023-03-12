#include <stdio.h>
#include "stack.h"
#include "../inc/rtest.h"
#include "mem.h"

int main(void)
{
    Stack_T s = stack_new();
    int i;
    int *iptr;
    void stack_print(Stack_T s);

    printf("stack is empty: %s (ok)\n", RTEST_STATUS_STR(stack_is_empty(s)));
    printf("push: ");
    for (i = 0; i < 5; i++) {
        printf("%d ", i);
        stack_push(s, MAKE_INT(i));
    }
    putchar('\n');

    printf("stack is empty: %s (err)\n", RTEST_STATUS_STR(stack_is_empty(s)));
    printf("top is %d? %s (ok) \n", i-1, RTEST_STATUS_STR(*((int *) stack_top(s)) == i-1));

    printf("pop: ");
    for (i = 0; i < 5; i++) {
        iptr = stack_pop(s);
        printf("%d ", *iptr);
    }
    putchar('\n');
    printf("stack is empty: %s (ok)\n", RTEST_STATUS_STR(stack_is_empty(s)));

    for (i = 0; i < 1024; i++)
        stack_push(s, MAKE_INT(i));
    printf("stack is empty: %s (err)\n", RTEST_STATUS_STR(stack_is_empty(s)));

    stack_print(s);

    /* printf("-- %p\n", stack_pop(s)); */

    printf("pop: ");
    while (!stack_is_empty(s)) {
        iptr = stack_pop(s);
        printf("%d ", *iptr);
    }
    putchar('\n');
    /* stack_make_empty(s); */
    printf("stack is empty: %s (ok)\n", RTEST_STATUS_STR(stack_is_empty(s)));

    stack_free(&s);

#ifdef MEM_CHK
    mem_print_allocated();
#endif
    return 0;
}
