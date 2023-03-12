#include <stdio.h>
#include <except.h>

Except_T const Test_Faild = { " fuck " };

int main(int argc, char *argv[])
{
    RAISE(Test_Faild);
    return 0;
}
