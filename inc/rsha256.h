#ifndef RSHA256_H
#define RSHA256_H

#include <stdint.h>

#define RSHA256_DIGEST_SIZE (256 / 8)

#define RSHA256_BLOCK_SIZE (512 / 8)

#define T RSha256_T

#ifdef __cplusplus
extern "C" {
#endif

typedef struct T *T;
struct T
{
    unsigned int  tot_len;
    unsigned int  len;
    unsigned char block[2 * RSHA256_BLOCK_SIZE];
    uint32_t      h[8];
};

extern void rsha256_init(T ctx);
extern void rsha256_update(T ctx, unsigned char const *message, unsigned int len);
extern void rsha256_final(T ctx, unsigned char *digest);
extern void rsha256(unsigned char const *message, unsigned int len,
                    unsigned char *digest);

#ifdef __cplusplus
}
#endif

#undef T

#endif /* end of include guard: RSHA256_H */
