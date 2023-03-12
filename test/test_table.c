#include <stdio.h>
#include <limits.h>
#include <string.h>
#include "mem.h"
#include "atom.h"
#include "table.h"
#include "../inc/rtest.h"

int cmpint(void const *x, void const *y)
{
    if (*(int *) x > *(int *) y)
        return 1;
    else if (*(int *) x < *(int *) y)
        return -1;
    else 
        return 0;
}

void apply_print(void const *key, void **val, void *cl)
{
    printf("key: %s\tval: %d\n", (char const *) key, **(int **) val);
}

int main(int argc, char *argv[])
{
    Table_T tab = table_new(0, NULL, NULL);
    int i;
    char const *key;
    void *val;
    for (i = 0; i < 380; i++) {
        key = atom_int(i);
        val = NEW_INT(i);
        fflush(stdout);
        table_put(tab, key, val);
    }

    table_map(tab, apply_print, NULL);

    printf("\n\n\n\n");

    printf("find 5: %s [ok] (%d)\n", RTEST_STATUS_STR(*(int *) table_get(tab, atom_int(5)) == 5), 
                *(int *) table_get(tab, atom_int(5)));
    table_put(tab, atom_int(5), NEW_INT(9999));
    printf("find 5: %s [ok] (%d)\n", RTEST_STATUS_STR(*(int *) table_get(tab, atom_int(5)) == 9999), 
                *(int *) table_get(tab, atom_int(5)));

#ifdef MEM_CHK
    print_allocated();
    print_freed();
#endif

    return 0;
}
