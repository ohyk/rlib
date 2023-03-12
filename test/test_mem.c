#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include "../inc/rtest.h"
#include "rmem.h"

static size_t Memroy_counter;

int get_random_memory(int max)
{
    int n = uniform(max) + 1;
    void *x = RALLOC(n);
    if (x)
        Memroy_counter += n;
    return x != NULL;
}

long get_number(char const *str)
{
    char *endp = NULL;
    long n;
    errno = 0;

    if ((n = strtol(str, &endp, 10)) == 0 && errno == EINVAL) {
        perror(__FILE__);
        assert(0);
    }
    if (errno == ERANGE)
        assert(0);
    if (endp == str)
        assert(0);
    return n;
}

int main(int argc, char *argv[])
{
    int count;
    int max;
    if (argc != 3)
        return 1;

    max = get_number(argv[1]);
    count = get_number(argv[2]);

    /* RMEM_ENV_PUSH(Rmem_Env_Arena); */

    for ( ; count-- > 0; ) 
        get_random_memory(max);

    fprintf(stderr, "Counter: %ld\n", Memroy_counter);
    return 0;
}
