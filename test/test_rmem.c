#include <stdio.h>
#include <string.h>
#include "../inc/rtest.h"
#include "rmem.h"

int main(int argc, char *argv[])
{

    int *mn = RALLOC(sizeof (int));
    *mn = 0;
    printf("default env: %s (ok)\n", RTEST_STATUS_STR(rmem_env_check() == Rmem_Env_Mem));
    rmem_print();
    printf("\n\n\n");

    int i;
    char *ms = RCALLOC(1, 1024);
    for (i = 0; i < 1024 && ms[i] == '\0'; i++)
        continue;
    printf("mem calloc: %s (ok)\n", RTEST_STATUS_STR(i == 1024));

    RRESIZE(ms, 512);
    rmem_print();

    RFREE(ms);
    rmem_print();

    RMEM_ENV_PUSH(Rmem_Env_Arena);
    int *an = RALLOC(sizeof (*an));
    rmem_print();
    printf("---- \n\n");

    for (i = 0; i < 5; i++)
        RALLOC(uniform(4096));
    rmem_print();
    printf("---- \n\n");

    RFREE(an);
    rmem_print();

    RMEM_ENV_POP();
    printf("env pop: %s (ok)\n", RTEST_STATUS_STR(rmem_env_check() == Rmem_Env_Mem));
    /* RMEM_ENV_POP(); */

    RMEM_ENV_PUSH(Rmem_Env_Arena);
    printf("env push: %s (ok)\n", RTEST_STATUS_STR(rmem_env_check() == Rmem_Env_Arena));
    int *a1n = RALLOC(sizeof (*a1n));
    rmem_print();

    RMEM_ENV_PUSH(Rmem_Env_Mem);
    printf("env push: %s (ok)\n", RTEST_STATUS_STR(rmem_env_check() == Rmem_Env_Mem));
    int *m2n = RALLOC(sizeof (*m2n));
    rmem_print();

    RMEM_ENV_PUSH(Rmem_Env_Arena);
    printf("env push: %s (ok)\n", RTEST_STATUS_STR(rmem_env_check() == Rmem_Env_Arena));
    int *a3n = RCALLOC(1, sizeof (*a3n));
    rmem_print();

    RMEM_ENV_POP();
    printf("\nenv pop: %s (ok)\n", RTEST_STATUS_STR(rmem_env_check() == Rmem_Env_Mem));

    RMEM_ENV_POP();
    printf("\nenv pop: %s (ok)\n", RTEST_STATUS_STR(rmem_env_check() == Rmem_Env_Arena));

    RMEM_ENV_POP();
    printf("\nenv pop: %s (ok)\n", RTEST_STATUS_STR(rmem_env_check() == Rmem_Env_Mem));

    rmem_print();

    /* RMEM_ENV_POP(); */
    RMEM_ENV_PUSH(999);

    return 0;
}
