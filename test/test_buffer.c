#include <stdio.h>
#ifdef __WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include <fcntl.h>
#include <string.h>
#include "buffer.h"
#include "rtest.h"

#define ARLENS(ar) (sizeof ((ar)) / sizeof ((ar)[0]))

static void output_empty(Buffer_Output_T obp)
{
    for ( ; buf_output_use_length(obp) > 0; buf_unputchar()) continue;
    fprintf(stderr, "buf empty is [%s]\n", 
            RTEST_STATUS_STR(buf_output_use_length(obp) == 0));
}

int main(int argc, char *argv[])
{
#ifdef FILE_FILTER
    int fd;
    char buf[BUFSIZ];

    if (argc != 2)
        return 1;

    if ((fd = open(argv[1], O_RDONLY)) == -1) {
        perror(__func__);
        return 1;
    }

    buf_input_init(&Global_Input_Buffer, fd);

    while (buf_getline(BUFSIZ, buf) != EOF) {
        printf("%s", buf);
    }
#endif

#define STR_LEN 32
    char line[] = "this is a line\n";
    char line2[] = "open asdf\nsecond line\n";
    char str[STR_LEN];
    char *res = NULL;
    int c;
    long n;
    Buffer_Output_T obp = &Global_Output_Buffer;

    fprintf(stderr, "rtest is [%s]\n", RTEST_STATUS_STR(1));
    fprintf(stderr, "rtest is [%s]\n", RTEST_STATUS_STR(0));

    fprintf(stderr, "1 of test string [%s] array length: %lld\n", 
            line,
            ARLENS(line));

    fprintf(stderr, "2 of test string [%s] array length: %lld\n", 
            line2,
            ARLENS(line2));

    /* test output part */
    buf_output_init(obp);

    res = buf_output_get(obp);
    fprintf(stderr, "buf addr: %p\n", res);
    fprintf(stderr, "buf use length is [%s]\n", 
            RTEST_STATUS_STR(buf_output_use_length(obp) == 0));

    buf_putchar('a');
    fprintf(stderr, "buf putchar is [%s]\n", 
            RTEST_STATUS_STR(buf_output_use_length(obp) == 1 && 
                *buf_output_get(obp) == 'a'));

    c = buf_unputchar();
    fprintf(stderr, "buf unputchar is [%s] --- c is [%c]\n", RTEST_STATUS_STR(c == 'a'), c);

    fprintf(stderr, "buf use length: %ld [0]\n", buf_output_use_length(obp));

#define TEST_PUTBLOCK_LEN 4
    n = buf_putblock(line, TEST_PUTBLOCK_LEN);
    fprintf(stderr, "buf putblock return is [%s] --- [%ld]\n", 
            RTEST_STATUS_STR(n == TEST_PUTBLOCK_LEN), n);
    fprintf(stderr, "buf putblock is [%s]\n", 
            RTEST_STATUS_STR(memcmp(res, line, TEST_PUTBLOCK_LEN) == 0));

    /* empty  */
    output_empty(obp);

    n = buf_putblock(line2, ARLENS(line2));
    fprintf(stderr, "buf putblock is [%s] --- return [%ld]\n", 
            RTEST_STATUS_STR(memcmp(res, line2, ARLENS(line2)) == 0 &&
                ARLENS(line2) == buf_output_use_length(obp)), n);


    /* empty  */
    output_empty(obp);

    n = buf_putstr(line, ARLENS(line));
    fprintf(stderr, "buf putstr is [%s] --- return [%ld]\n", 
            RTEST_STATUS_STR(memcmp(res, line, ARLENS(line)-1) == 0 &&
                n == ARLENS(line)-1), n); /* decrement ARLENS for '\0' */


    /* empty  */
    output_empty(obp);

#define TEST_LINE2_FIRST_LB 10
    n = buf_putline(line2, ARLENS(line2));
    fprintf(stderr, "buf putline is [%s] --- return [%ld]\n", 
            RTEST_STATUS_STR(memcmp(res, line2, TEST_LINE2_FIRST_LB-1) == 0 &&
                n == TEST_LINE2_FIRST_LB-1 && 
                buf_output_use_length(obp) == TEST_LINE2_FIRST_LB-1), n);


    buf_putchar('\n');

    buf_putline(line, ARLENS(line));
    /* buf is -- [open asdf\nthis is a line] */
    n = buf_unputblock(str, ARLENS(line)-2);
    fprintf(stderr, "buf unputblock is [%s] --- return [%ld]\n", 
            RTEST_STATUS_STR(memcmp(str, line, ARLENS(line)-2) == 0), n);
    if (n < STR_LEN)
        str[n] = '\0';
    fprintf(stderr, "str is [%s]\n", str);


    /* buf is -- [open asdf\n] */
    buf_putline(line, ARLENS(line));
    /* buf is -- [open asdf\nthis is a line] */
    n = buf_unputblock_delm(NULL, 123, '\n');
    fprintf(stderr, "buf unputblock block is NULL: [%s]\n", 
            RTEST_STATUS_STR(buf_output_use_length(obp) == TEST_LINE2_FIRST_LB-1));

    /* buf is -- [open asdf] */

    /* assert test */
    /* buf_putblock(line2, 0); */

    buf_putchar('\0');
    n = buf_output_use_length(obp);
    res = buf_output_get_dispose(obp);

    fprintf(stderr, "buf dispose is [%s]\n", RTEST_STATUS_STR(obp->buf == NULL));
    fprintf(stderr, "res: [%s]\n", res);

    return 0;
}
