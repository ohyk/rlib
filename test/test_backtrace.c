#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>

void func1(void)
{
    printf("-- backtrace --\n");
    void *call_stack[128];
    int i, size = backtrace(call_stack, 128);
    char **symbols = backtrace_symbols(call_stack, size);
    for (i = 0; i < size; i++) {
        printf("%s\n", symbols[i]);
    }
    free(symbols);
    return;
}

void func2(void)
{
    func1();
    return;
}

void func3(void)
{
    func2();
    return;
}

int main(int argc, char *argv[])
{
    func3();
    return 0;
}
