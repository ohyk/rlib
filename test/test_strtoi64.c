#include <stdio.h>
#include <errno.h>

extern int64_t strtoi64(const char *str, char **endptr, int base);

int main(int argc, char *argv[])
{
    char buf[BUFSIZ];
    char *endp;

    while (fgets(buf, BUFSIZ, stdin) != NULL) {
        errno = 0;
        printf("i64: %lld\n", strtoi64(buf, &endp, 8));
        perror(argv[0]);
        printf("endpt: %s\n", endp);
    }
    return 0;
}
