#include <stdio.h>
#include "rrandom.h"

int main(int argc, char *argv[])
{
    int i;

    printf("zero2one: \n");
    for (i = 0; i < 10; ++i)
        printf("%f\n", rrandom_uniform_zero2one());

    printf("real: \n");
    for (i = 0; i < 10; ++i)
        printf("%f\n", rrandom_uniform_real(-20.45, 75.39));

    printf("int: \n");
    for (i = 0; i < 10; ++i)
        printf("%d\n", rrandom_uniform_int(5, 50));

    printf("r32: \n");
    for (i = 0; i < 10; ++i)
        printf("%d\n", rrandom_rand32());

    printf("i32:\n");
    for (i = 0; i < 10; ++i)
        printf("%d\n", rrandom_uniform_int32(32767, 65535));

    return 0;
}
