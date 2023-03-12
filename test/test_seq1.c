#include <stdio.h>
#include <rtest.h>
#include "seq.h"

int main(int argc, char *argv[])
{
    int i;
    Seq_T seq = seq_new(0);
    for (i = 0; i < 10240; i++) {
        seq_addhi(seq, MAKE_INT(i));
        /* seq_addlo(seq, MAKE_INT(i)); */
    }
    printf("seq length: %d\n", seq_length(seq));
    return 0;
}
