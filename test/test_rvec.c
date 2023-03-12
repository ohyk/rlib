#include "rvec.h"

#include <stdio.h>

int main(int argc, char *argv[])
{
    rvec_vector_type(int) v = NULL;
    
    for (int i = 0; i < 10; ++i)
        rvec_push_back(v, i);

    rvec_erase(v, 2);

    rvec_pop_back(v);

    printf("pointer : %p\n", (void *)v);
    printf("capacity: %zu\n", rvec_capacity(v));
    printf("size    : %zu\n", rvec_size(v));

    if (v) {
        int *it;
        int i = 0;
        for (it = rvec_begin(v); it != rvec_end(v); ++it)
            printf("v[%d] = %d\n", i++, *it);
    }

    if (v) {
        size_t i;
        for (i = 0; i < rvec_size(v); ++i)
            printf("v[%zu] = %d\n", i, v[i]);
    }

    rvec_free(v);

    return 0;
}
