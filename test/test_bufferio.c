#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "arena.h"
#include "mem.h"
#include "bufferio.h"
#include "../inc/rtest.h"

long get_file_length(FILE *fp)
{
    long len;
    errno = 0;
    /* file start */
    if (fseek(fp, 0, SEEK_SET) != 0) {
        perror(__func__);
        return 3;
    }
    if (fseek(fp, 0, SEEK_END) != 0) {
        perror(__func__);
        return 3;
    }
    if ((len = ftell(fp)) == -1) {
        perror(__func__);
        return 3;
    }
    /* reset start */
    if (fseek(fp, 0, SEEK_SET) != 0) {
        perror(__func__);
        return 3;
    }
    return len;
}

void print_inputbuffer_status(void *ibp)
{
    fprintf(stderr, "buffer: %p\n", ((Input_Buffer_T *) ibp)->buf);
    fprintf(stderr, "count: %d\n", ((Input_Buffer_T *) ibp)->count);
    fprintf(stderr, "offset: %d\n", ((Input_Buffer_T *) ibp)->offset);
}

void print_outbuffer_status(void *obp)
{
    fprintf(stderr, "buffer: %p\n", ((Output_Buffer_T *) obp)->buf);
    fprintf(stderr, "length: %ld\n", ((Output_Buffer_T *) obp)->length);
    fprintf(stderr, "offset: %ld\n", ((Output_Buffer_T *) obp)->offset);
}

void print_buffer_status(char const *title, void pt(void *cl), void *cl)
{
    fprintf(stderr, "-- %s --\n", title);
    pt(cl);
    fprintf(stderr, "-- %s end --\n", title);
}

int main(int argc, char *argv[])
{
    Arena_T arena = arena_new();
    int i, c;
    long fpsize;
    long n;
    char *str;
    char *rstr;
    char *bstr;
    FILE *fp;

    if (argc != 2) 
        return 1;
    if ((fp = fopen(argv[1], "r")) == NULL) {
        perror(argv[0]);
        return 2;
    }

    fpsize = get_file_length(fp);
    str = ARENA_ALLOC(arena, fpsize+1);
    rstr = ARENA_ALLOC(arena, fpsize+1);
    bstr = ARENA_ALLOC(arena, fpsize+1);

    Global_Input_Buffer.in = fp;
    printf("file length: %ld\n", fpsize);
    for (i = 0; i < fpsize && (c = buf_getchar()) != EOF; i++)
        str[i] = c;
    str[i] = '\0';
    printf("%s\n", str);
    
    for (i = 0; i < fpsize && i < INPUT_BUF_SIZE; i++)
        buf_ungetchar(str[i]);

    for (i = 0; i < fpsize && (c = buf_getchar()) != EOF; i++)
        rstr[i] = c;
    rstr[i] = '\0';
    printf("%s\n", rstr);

    if (buf_getchar() == EOF)
        printf("EOF\n");

    if (fseek(fp, 0, SEEK_SET) != 0) {
        perror(__func__);
        return 3;
    }
    /* printf("kkk\n"); */
    /* while ((c = fgetc(fp)) != EOF) */
        /* putchar(c); */

    while ((c = buf_getchar()) != EOF)
        putchar(c);

    fclose(fp);

    /* reset file */
    if ((fp = fopen(argv[1], "r")) == NULL) {
        perror(argv[0]);
        return 2;
    }


    if ((n = buf_getblock(fpsize, bstr)) != fpsize) {
        printf("get block error\n");
        printf("%ld\n", n);
        return 4;
    }

    bstr[fpsize] = '\0';
    printf("%s", bstr);

    if (fseek(fp, -20, SEEK_END) != 0) {
        perror(__func__);
        return 3;
    }

    while ((c = buf_getchar()) != EOF)
        putchar(c);

    if (fseek(fp, 0, SEEK_SET) != 0) {
        perror(__func__);
        return 3;
    }

    c = buf_getchar();
    printf("%c\n", c);

    if ((n = buf_getblock(fpsize, bstr)) < 0) {
        printf("get block error\n");
        return 4;
    }
    bstr[n] = '\0';
    printf("%s", bstr);
    printf("n: %ld\n", n);

    printf("get block buffer empty and EOF: %s [ok]\n", 
            RTEST_STATUS_STR(buf_getblock(10, bstr) == 0));

    printf("\nunputchar/getchar\n");
    if (fseek(fp, 0, SEEK_SET) != 0) {
        perror(__func__);
        return 3;
    }
    int c2;
    c = buf_getchar();
    printf("getchar: %c\n", c);
    buf_ungetchar(c);
    c2 = buf_getchar();
    printf("ungetchar: %s [ok] (%c)\n", RTEST_STATUS_STR(c2 == c), c2);

    c = buf_getchar();
    printf("getchar: %c\n", c);
    buf_ungetchar(c);
    c2 = buf_getchar();
    printf("ungetchar: %s [ok] (%c)\n", RTEST_STATUS_STR(c2 == c), c2);
    printf("\n\n");

    n = buf_getblock(20, bstr);
    printf("getblock n: %s [ok] (%ld)\n", RTEST_STATUS_STR(n == 20), n);


    /* output part */
    char *outstr = ARENA_ALLOC(arena, fpsize);
    memset(outstr, '\0', fpsize);
    printf("output\n");
    buf_putchar('1');
    buf_putchar('2');
    buf_putblock(5, "abcde");
    buf_putblock(fpsize, rstr);
    buf_putblock(fpsize, str);
    printf("--unputblock--\n");
    printf("length: %d\n%s", buf_unputblock(fpsize, outstr), outstr);
    printf("length: %d\n%s", buf_unputblock(fpsize*4, outstr), outstr);
    printf("--unputblock end--\n");
    buf_putchar('\n');

    n = buf_printf_outbuffer(&Global_Output_Buffer, 1024, "KKKKK%2x%2x", 'a', 'A');
    print_buffer_status("printf_buffer", print_outbuffer_status, &Global_Output_Buffer);
    buf_putchar('\n');
    n = buf_printf(1, "n");
    print_buffer_status("printf_buffer", print_outbuffer_status, &Global_Output_Buffer);
    n = buf_printf(4, "%2x\n", 'a');
    buf_putchar('\n');

    for (i = 0; i < fpsize; i++) {
        buf_printf(3, "%02x", outstr[i]);
        if (((i+1) & 1) == 0)
            buf_putchar(' ');
    }
    buf_putchar('\n');
    print_buffer_status("printf", print_outbuffer_status, &Global_Output_Buffer);

    buf_fill_char_outbuffer(&Global_Output_Buffer, 7, '1');
    print_buffer_status("fill char", print_outbuffer_status, &Global_Output_Buffer);
    buf_fill_char_outbuffer(&Global_Output_Buffer, 1, '2');
    buf_fill_char(12, 'f');
    buf_putchar('\n');

    buf_flush();

#ifdef MEM_CHK
    mem_print_allocated();
    mem_print_freed();
#endif

    return 0;
}
