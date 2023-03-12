#include <stdio.h>
#include "table.h"
#include "atom.h"
#include "rtest.h"

extern void print_table_info(Table_T table);

void print_key_val(void const *key, void **val, void *cl)
{
    printf("key: %s --- val: %s\n", (char const *) key,  (char const *)*val);
}

int main(int argc, char *argv[])
{
    Table_T tab = table_new(0, NULL, NULL);
    int i;

    fprintf(stderr, "empty table\n");
    print_table_info(tab);

    fprintf(stderr, "append 0-3 to table...\n");
    for (i = 0; i < 4; i++)
        table_put(tab, atom_int(i), (void *) atom_int(i));

    table_map(tab, print_key_val, NULL);

    print_table_info(tab);

    fprintf(stderr, "append 4-10 to table... (change table size)\n");
    for (i = 4; i < 11; i++)
        table_put(tab, atom_int(i), (void *) atom_int(i));

    table_map(tab, print_key_val, NULL);

    print_table_info(tab);

    fprintf(stderr, "changed table size after value status...\n");
    for (i = 0; i < 12; i++)
        fprintf(stderr, "changed table size after find %2d is [%s]\n", i,
                RTEST_STATUS_STR(table_get(tab, atom_int(i)) == atom_int(i)));

    /* append to expand */
    fprintf(stderr, "append 11-32 to table... (change table size)\n");
    for (i = 11; i < 33; i++)
        table_put(tab, atom_int(i), (void *) atom_int(i));

    print_table_info(tab);
    fprintf(stderr, "changed table size after value status...\n");
    for (i = 0; i < 34; i++)
        fprintf(stderr, "changed table size after find %2d is [%s]\n", i,
                RTEST_STATUS_STR(table_get(tab, atom_int(i)) == atom_int(i)));

    return 0;
}
