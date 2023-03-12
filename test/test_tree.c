#include <stdio.h>
#include "tree.h"
#include "mem.h"

static int *make_int(int n)
{
    int *iptr = ALLOC(sizeof(int));
    *iptr = n;
    return iptr;
}

static int cmp(void *a, void *b)
{
    return *((int *) a) - *((int *)b);
}

static void pt(void *x)
{
    printf("%d\n", *((int*)x));
}

int main(int argc, char *argv[])
{
    int i;
    Tree_T t = NULL;

    printf("Insert tree: ");
    for (i = 0; i < 10; i++) {
        printf("%d ", i);
        t = tree_insert(make_int(i), cmp, t);
    }
    putchar('\n');

    print_tree(pt, t);
    putchar('\n');
    printf("root: %d\n", *((int *) tree_retrieve(t)));
    printf("left: \n");
    print_left(pt, t);
    printf("right: \n");
    print_right(pt, t);
    return 0;
}
