/*============================== Test Part ===============================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rsha256.h"

void test(char const *vector, unsigned char *digest, unsigned int digest_size)
{
    char output[2 * RSHA256_DIGEST_SIZE + 1];
    int  i;

    output[2 * digest_size] = '\0';

    for (i = 0; i < (int) digest_size; i++) {
        sprintf(output + 2 * i, "%02x", digest[i]);
    }

    printf("H: %s\n", output);
    if (strcmp(vector, output)) {
        fprintf(stderr, "Test failed.\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    static const char *vector[3] = {
        "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad",
        "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1",
        "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0",
    };

    static char const message1[] = "abc";
    static char const message2a[] =
        "abcdbcdecdefdefgefghfghighijhi"
        "jkijkljklmklmnlmnomnopnopq";
    unsigned char *message3;
    unsigned int   message3_len = 1000000;
    unsigned char  digest[RSHA256_DIGEST_SIZE];

    message3 = malloc(message3_len);
    if (message3 == NULL) {
        fprintf(stderr, "Can't allocate memory\n");
        return -1;
    }
    memset(message3, 'a', message3_len);

    printf("SHA-2 FIPS 180-2 Validation tests\n\n");

    printf("SHA-256 Test vectors\n");

    rsha256((unsigned char const *) message1, strlen(message1), digest);
    test(vector[0], digest, RSHA256_DIGEST_SIZE);
    rsha256((unsigned char const *) message2a, strlen(message2a), digest);
    test(vector[1], digest, RSHA256_DIGEST_SIZE);
    rsha256(message3, message3_len, digest);
    test(vector[2], digest, RSHA256_DIGEST_SIZE);
    printf("\n");

    printf("All tests passed.\n");
    return 0;
}

/*============================ Test Part End =============================*/
