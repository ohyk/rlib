#include <float.h>
#include "fmt.h"

void cvt_test(int code, va_list *app, int put(int c, void *cl), void *cl,
        unsigned char flags[], int width, int precision)
{
}

int main(int argc, char *argv[])
{
    fmt_fprint(stdout, "Fmt test start\n");
    fmt_print("%d\n", 123);
    fmt_print("%.99f\n", DBL_MAX);
    return 0;
}
