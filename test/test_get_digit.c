#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "assert.h"
#include "mem.h"

#define BUFFER_CHAR_MAX 128
static int Buffer[BUFFER_CHAR_MAX];
static int Bufsp;

static int fgetchar(FILE *fp)
{
    if (Bufsp > 0)
        return Buffer[--Bufsp];
    else
        return fgetc(fp);
}

static void fungetchar(int c)
{
    if (Bufsp < BUFFER_CHAR_MAX)
        Buffer[Bufsp++] = c;
    else
        assert(!"getchar buffer full");
}

double *json_get_digit(FILE *fp)
{
    char buf[51];
    char *endp = NULL;
    double d;
    double *dp;
    int c, i;

    /* collect chars */
    i = 0;
    while (i < 51 && ((c = fgetchar(fp)) == '-' || c == '+' || isdigit(c) || 
            c == '.' || c == 'E' || c == 'e'))
        buf[i++] = c;
    buf[i] = '\0';
    fungetchar(c);

    errno = 0;
    if ((d = strtod(buf, &endp)) == 0 && errno == ERANGE) {
        assert(!"underflow");
    }
    if (errno == ERANGE) {
        assert(!"overflow");
    }
    if (endp == buf) {
        assert(!"string not work");
    }
    NEW(dp);
    *dp = d;
    return dp;
}

static int escape_sequence(FILE *fp)
{
    int c;
    if ((c = fgetchar(fp)) == '\\')
        switch ((c = fgetchar(fp))) {
            case '\\':
                c = '\\';
                break;
            case '"':
                c = '"';
                break;
            case 'b':
                c = '\b';
                break;
            case 'n':
                c = '\n';
                break;
            case 't':
                c = '\t';
                break;
            case 'f':
                c = '\f';
                break;
            case 'r':
                c = '\r';
                break;
            case 'u':
                fungetchar(c);
                fungetchar('\\');
                c = -2;
                break;
            default:
                fungetchar(c);
                fungetchar('\\');
                c = -1;
                break;
        }
    return c;
}

char *json_get_string(FILE *fp)
{
    int i, c;
    char buf[3];
    char *str = NULL;
    int count = 0;

    for (i = 0; i < 3 && (c = fgetchar(fp)) != EOF && c != '"'; i++) {
        if (c == '\\') {
            fungetchar(c);
            c = escape_sequence(fp);
        }
        /* if (c == -1) { */
            /* assert(!"escape sequence fault"); */
        /* } */
        if ((c == -1 || c == -2) && i+1 < 3) { /* Unicode */
            buf[i++] = fgetchar(fp);
            count++;
            buf[i] = fgetchar(fp);
        } else
            buf[i] = c;

        count++;
        /* buf is full */
        if (i+1 == 3 && str == NULL) {
            str = ALLOC(i+1);
            memcpy(str, buf, i+1);
            i = -1;
        } else if (i+1 == 3 && str != NULL) {
            RESIZE(str, count);
            memcpy(str+(count-(i+1)), buf, i+1);
            i = -1;
        }
    }

    if (str == NULL) {
        str = ALLOC(i+1);
        memcpy(str, buf, i);
    } else if (str && i != 0) {
        RESIZE(str, count+1);
        memcpy(str+(count-i), buf, i);
    } else {
        RESIZE(str, count+1);
    }
    str[count] = '\0';
    return str;
}

typedef int *Boolean_T;
enum { False, True };
char *False_Str = "false";
char *True_Str = "true";
Boolean_T json_get_boolean(FILE *fp)
{
    Boolean_T b;
    int maybe = False;
    char buf[5];
    int c, i;

    for (i = 0; i < 4 && (c = fgetchar(fp)) != EOF; i++)
        buf[i] = c;
    buf[i] = '\0';

    if (memcmp(buf, True_Str, sizeof (buf)) == 0)
        maybe = True;
    else if (memcmp(buf, False_Str, sizeof(buf)-1) == 0 &&
            (c = fgetchar(fp)) == 'e') {
        maybe = False;
    } else
        assert(!"bad string");
    NEW(b);
    *b = maybe;
    return b;
}

static char *Null_Str = "null";
void *json_get_null(FILE *fp)
{
    int i, c;
    int nslen = strlen(Null_Str);
    char buf[nslen+1];
    for (i = 0; i < nslen && (c = fgetchar(fp)) != EOF; i++)
        buf[i] = c;
    buf[i] = '\0';

    if (memcmp(buf, Null_Str, nslen+1) != 0) {
        assert(!"bad sting");
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    /* double *dp = NULL; */
    int c;
    char *str = NULL;
    /* dp = json_get_digit(stdin); */
    /* printf("%lf\n", *dp); */

    if ((c = fgetchar(stdin)) == '"')
        str = json_get_string(stdin);
    else 
        return 0;

    printf("%s\n", str);
    printf("%ld\n", strlen(str));

    /* Boolean_T b; */
    /* b = json_get_boolean(stdin); */
    /* printf("%d\n", *b); */

    /* if (json_get_null(stdin) == NULL) { */
        /* printf("get null\n"); */
    /* } else { */
        /* printf("get null fault\n"); */
    /* } */

#ifdef MEM_CHK
    print_allocated();
    print_freed();
#endif
    return 0;
}
