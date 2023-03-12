#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "mem.h"
#include "rjson.h"
#include "bufferio.h"

char *get_file_data(FILE *fp, unsigned long *size)
{
    char *f = NULL;
    unsigned long gn = 0;
    unsigned long n  = 0;
    char buf[BUFSIZ];
    assert(fp);
    assert(size);


    while (!feof(fp)) {
        gn = fread(buf, 1, BUFSIZ, fp);
        if (!f) {
            f = ALLOC(gn);
            memcpy(f, buf, gn);
        } else {
            RESIZE(f, gn+n);
            memcpy(f+n, buf, gn);
        }
        /* printf("%ld\n", gn); */
        n += gn;
    }
    *size = n;
    return f;
}

int main(int argc, char *argv[])
{
    FILE *fp;
    /* char *file; */
    Json_T jt;
    /* char *out; */
    /* unsigned long size; */
    char *out;

    if (argc == 2) {
        if ((fp = fopen(argv[1], "r")) == NULL) {
            perror(argv[0]);
            return 1;
        }
    } else 
        fp = stdin;

    /* file = get_file_data(fp, &size); */

    /* while ((json = cJSON_Parse(file)) != NULL) { */
        /* printf("%s\n", cJSON_Print(json)); */
    /* } */

    if ((jt = json_parse(fp)) == NULL) {
        printf("parse faild\n");
    }
    /* printf("%d\n", jt->type); */
    out = json_print(jt, 0);
    /* json_print(jt, 0); */
    printf("%s", out);
    /* buf_flush_fp(stdout); */

    /* if ((json = cJSON_Parse(file)) == NULL) { */
        /* printf("parse faild\n"); */
    /* } */

    /* if ((out = cJSON_Print(json)) != NULL) { */
        /* printf("%s\n", out); */
    /* } */

    return 0;
}
