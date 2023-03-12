#include <stdio.h>
#include "arith.h"
#include "../inc/rtest.h"

int main(int argc, char *argv[])
{
    printf("-13/5: %s (-3)\n", RTEST_STATUS_STR(arith_div(-13, 5) == -3));
    printf("-13/5 = %d\n", -13/5);
    printf("-13%%5: %s (2)\n", RTEST_STATUS_STR(arith_mod(-13, 5) == 2));
    printf("-13%%5 = %d\n", -13%5);
    return 0;
}
