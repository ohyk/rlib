#include <stdio.h>
#include "list.h"
#include "mem.h"

void apply_print(void **x, void *cl)
{
    printf("%s\n", *(char **)x);
}

int main(int argc, char *argv[])
{
    char *strs[] = {
        "ABC",
        "DEF",
        "GHI",
        "OPQ",
        "RST",
        NULL
    };
    List_T l = list_list("ABC", 
            (void *) "DEF",
            (void *) "GHI",
            (void *) "OPQ",
            (void *) "RST",
            NULL);
    void *x;

    list_map(l, apply_print, NULL);
    putchar('\n');

    printf("%d\n\n", list_length(l));

    l = list_pop(l, &x);
    printf("%s\n", (char *) x);
    putchar('\n');
    list_map(l, apply_print, NULL);
    putchar('\n');

    printf("%d\n\n", list_length(l));

    l = list_push(l, "HEAD");
    list_map(l, apply_print, NULL);
    putchar('\n');

    l = list_reverse(l);
    list_map(l, apply_print, NULL);
    putchar('\n');

    printf("%d\n\n", list_length(l));

    list_free(&l);
    
#ifdef MEM_CHK
    print_allocated();
    print_freed();
#endif

    return 0;
}
