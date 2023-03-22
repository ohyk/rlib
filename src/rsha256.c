#include "rsha256.h"

#include <stdint.h>
#include <string.h>

#include "rassert.h"

#define T RSha256_T

#define SHIFT_R(x, n) (x >> n)
#define ROTATE_R(x, n) ((x >> n) | (x << ((sizeof(x) << 3) - n)))
#define ROTATE_L(x, n) ((x << n) | (x >> ((sizeof(x) << 3) - n)))
#define CH(x, y, z) ((x & y) ^ (~x & z))
#define MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))

#define RSHA256_F1(x) (ROTATE_R(x, 2) ^ ROTATE_R(x, 13) ^ ROTATE_R(x, 22))
#define RSHA256_F2(x) (ROTATE_R(x, 6) ^ ROTATE_R(x, 11) ^ ROTATE_R(x, 25))
#define RSHA256_F3(x) (ROTATE_R(x, 7) ^ ROTATE_R(x, 18) ^ SHIFT_R(x, 3))
#define RSHA256_F4(x) (ROTATE_R(x, 17) ^ ROTATE_R(x, 19) ^ SHIFT_R(x, 10))

#define UNPACK32(x, str)                      \
    do {                                      \
        *((str) + 3) = (uint8_t) ((x));       \
        *((str) + 2) = (uint8_t) ((x) >> 8);  \
        *((str) + 1) = (uint8_t) ((x) >> 16); \
        *((str) + 0) = (uint8_t) ((x) >> 24); \
    } while (0)

#define PACK32(x, str)                                                        \
    do {                                                                      \
        *(x) = ((uint32_t) * ((str) + 3)) | ((uint32_t) * ((str) + 2) << 8) | \
               ((uint32_t) * ((str) + 1) << 16) |                             \
               ((uint32_t) * ((str) + 0) << 24);                              \
    } while (0)

#define RSHA256_SCR(i)                                                   \
    do {                                                                 \
        w[i] = RSHA256_F4(w[i - 2]) + w[i - 7] + RSHA256_F3(w[i - 15]) + \
               w[i - 16];                                                \
    } while (0)

#define RSHA256_EXP(a, b, c, d, e, f, g, h, j)                     \
    do {                                                           \
        x1 = wv[h] + RSHA256_F2(wv[e]) + CH(wv[e], wv[f], wv[g]) + \
             rsha256_k[j] + w[j];                                  \
        x2 = RSHA256_F1(wv[a]) + MAJ(wv[a], wv[b], wv[c]);         \
        wv[d] += x1;                                               \
        wv[h] = x1 + x2;                                           \
    } while (0)

uint32_t rsha256_h0[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                          0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

uint32_t rsha256_k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
    0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
    0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
    0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
    0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
    0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

static void rsha256_transf(T ctx, const unsigned char *message, unsigned blk_nb)
{
    uint32_t             w[64];
    uint32_t             wv[8];
    uint32_t             x1, x2;
    unsigned char const *sub_blk;
    unsigned             i;

    for (i = 0; i < blk_nb; ++i) {
        sub_blk = message + (i << 6);

        PACK32(&w[0], &sub_blk[0]);
        PACK32(&w[1], &sub_blk[4]);
        PACK32(&w[2], &sub_blk[8]);
        PACK32(&w[3], &sub_blk[12]);
        PACK32(&w[4], &sub_blk[16]);
        PACK32(&w[5], &sub_blk[20]);
        PACK32(&w[6], &sub_blk[24]);
        PACK32(&w[7], &sub_blk[28]);
        PACK32(&w[8], &sub_blk[32]);
        PACK32(&w[9], &sub_blk[36]);
        PACK32(&w[10], &sub_blk[40]);
        PACK32(&w[11], &sub_blk[44]);
        PACK32(&w[12], &sub_blk[48]);
        PACK32(&w[13], &sub_blk[52]);
        PACK32(&w[14], &sub_blk[56]);
        PACK32(&w[15], &sub_blk[60]);

        RSHA256_SCR(16);
        RSHA256_SCR(17);
        RSHA256_SCR(18);
        RSHA256_SCR(19);
        RSHA256_SCR(20);
        RSHA256_SCR(21);
        RSHA256_SCR(22);
        RSHA256_SCR(23);
        RSHA256_SCR(24);
        RSHA256_SCR(25);
        RSHA256_SCR(26);
        RSHA256_SCR(27);
        RSHA256_SCR(28);
        RSHA256_SCR(29);
        RSHA256_SCR(30);
        RSHA256_SCR(31);
        RSHA256_SCR(32);
        RSHA256_SCR(33);
        RSHA256_SCR(34);
        RSHA256_SCR(35);
        RSHA256_SCR(36);
        RSHA256_SCR(37);
        RSHA256_SCR(38);
        RSHA256_SCR(39);
        RSHA256_SCR(40);
        RSHA256_SCR(41);
        RSHA256_SCR(42);
        RSHA256_SCR(43);
        RSHA256_SCR(44);
        RSHA256_SCR(45);
        RSHA256_SCR(46);
        RSHA256_SCR(47);
        RSHA256_SCR(48);
        RSHA256_SCR(49);
        RSHA256_SCR(50);
        RSHA256_SCR(51);
        RSHA256_SCR(52);
        RSHA256_SCR(53);
        RSHA256_SCR(54);
        RSHA256_SCR(55);
        RSHA256_SCR(56);
        RSHA256_SCR(57);
        RSHA256_SCR(58);
        RSHA256_SCR(59);
        RSHA256_SCR(60);
        RSHA256_SCR(61);
        RSHA256_SCR(62);
        RSHA256_SCR(63);

        wv[0] = ctx->h[0];
        wv[1] = ctx->h[1];
        wv[2] = ctx->h[2];
        wv[3] = ctx->h[3];
        wv[4] = ctx->h[4];
        wv[5] = ctx->h[5];
        wv[6] = ctx->h[6];
        wv[7] = ctx->h[7];

        RSHA256_EXP(0, 1, 2, 3, 4, 5, 6, 7, 0);
        RSHA256_EXP(7, 0, 1, 2, 3, 4, 5, 6, 1);
        RSHA256_EXP(6, 7, 0, 1, 2, 3, 4, 5, 2);
        RSHA256_EXP(5, 6, 7, 0, 1, 2, 3, 4, 3);
        RSHA256_EXP(4, 5, 6, 7, 0, 1, 2, 3, 4);
        RSHA256_EXP(3, 4, 5, 6, 7, 0, 1, 2, 5);
        RSHA256_EXP(2, 3, 4, 5, 6, 7, 0, 1, 6);
        RSHA256_EXP(1, 2, 3, 4, 5, 6, 7, 0, 7);
        RSHA256_EXP(0, 1, 2, 3, 4, 5, 6, 7, 8);
        RSHA256_EXP(7, 0, 1, 2, 3, 4, 5, 6, 9);
        RSHA256_EXP(6, 7, 0, 1, 2, 3, 4, 5, 10);
        RSHA256_EXP(5, 6, 7, 0, 1, 2, 3, 4, 11);
        RSHA256_EXP(4, 5, 6, 7, 0, 1, 2, 3, 12);
        RSHA256_EXP(3, 4, 5, 6, 7, 0, 1, 2, 13);
        RSHA256_EXP(2, 3, 4, 5, 6, 7, 0, 1, 14);
        RSHA256_EXP(1, 2, 3, 4, 5, 6, 7, 0, 15);
        RSHA256_EXP(0, 1, 2, 3, 4, 5, 6, 7, 16);
        RSHA256_EXP(7, 0, 1, 2, 3, 4, 5, 6, 17);
        RSHA256_EXP(6, 7, 0, 1, 2, 3, 4, 5, 18);
        RSHA256_EXP(5, 6, 7, 0, 1, 2, 3, 4, 19);
        RSHA256_EXP(4, 5, 6, 7, 0, 1, 2, 3, 20);
        RSHA256_EXP(3, 4, 5, 6, 7, 0, 1, 2, 21);
        RSHA256_EXP(2, 3, 4, 5, 6, 7, 0, 1, 22);
        RSHA256_EXP(1, 2, 3, 4, 5, 6, 7, 0, 23);
        RSHA256_EXP(0, 1, 2, 3, 4, 5, 6, 7, 24);
        RSHA256_EXP(7, 0, 1, 2, 3, 4, 5, 6, 25);
        RSHA256_EXP(6, 7, 0, 1, 2, 3, 4, 5, 26);
        RSHA256_EXP(5, 6, 7, 0, 1, 2, 3, 4, 27);
        RSHA256_EXP(4, 5, 6, 7, 0, 1, 2, 3, 28);
        RSHA256_EXP(3, 4, 5, 6, 7, 0, 1, 2, 29);
        RSHA256_EXP(2, 3, 4, 5, 6, 7, 0, 1, 30);
        RSHA256_EXP(1, 2, 3, 4, 5, 6, 7, 0, 31);
        RSHA256_EXP(0, 1, 2, 3, 4, 5, 6, 7, 32);
        RSHA256_EXP(7, 0, 1, 2, 3, 4, 5, 6, 33);
        RSHA256_EXP(6, 7, 0, 1, 2, 3, 4, 5, 34);
        RSHA256_EXP(5, 6, 7, 0, 1, 2, 3, 4, 35);
        RSHA256_EXP(4, 5, 6, 7, 0, 1, 2, 3, 36);
        RSHA256_EXP(3, 4, 5, 6, 7, 0, 1, 2, 37);
        RSHA256_EXP(2, 3, 4, 5, 6, 7, 0, 1, 38);
        RSHA256_EXP(1, 2, 3, 4, 5, 6, 7, 0, 39);
        RSHA256_EXP(0, 1, 2, 3, 4, 5, 6, 7, 40);
        RSHA256_EXP(7, 0, 1, 2, 3, 4, 5, 6, 41);
        RSHA256_EXP(6, 7, 0, 1, 2, 3, 4, 5, 42);
        RSHA256_EXP(5, 6, 7, 0, 1, 2, 3, 4, 43);
        RSHA256_EXP(4, 5, 6, 7, 0, 1, 2, 3, 44);
        RSHA256_EXP(3, 4, 5, 6, 7, 0, 1, 2, 45);
        RSHA256_EXP(2, 3, 4, 5, 6, 7, 0, 1, 46);
        RSHA256_EXP(1, 2, 3, 4, 5, 6, 7, 0, 47);
        RSHA256_EXP(0, 1, 2, 3, 4, 5, 6, 7, 48);
        RSHA256_EXP(7, 0, 1, 2, 3, 4, 5, 6, 49);
        RSHA256_EXP(6, 7, 0, 1, 2, 3, 4, 5, 50);
        RSHA256_EXP(5, 6, 7, 0, 1, 2, 3, 4, 51);
        RSHA256_EXP(4, 5, 6, 7, 0, 1, 2, 3, 52);
        RSHA256_EXP(3, 4, 5, 6, 7, 0, 1, 2, 53);
        RSHA256_EXP(2, 3, 4, 5, 6, 7, 0, 1, 54);
        RSHA256_EXP(1, 2, 3, 4, 5, 6, 7, 0, 55);
        RSHA256_EXP(0, 1, 2, 3, 4, 5, 6, 7, 56);
        RSHA256_EXP(7, 0, 1, 2, 3, 4, 5, 6, 57);
        RSHA256_EXP(6, 7, 0, 1, 2, 3, 4, 5, 58);
        RSHA256_EXP(5, 6, 7, 0, 1, 2, 3, 4, 59);
        RSHA256_EXP(4, 5, 6, 7, 0, 1, 2, 3, 60);
        RSHA256_EXP(3, 4, 5, 6, 7, 0, 1, 2, 61);
        RSHA256_EXP(2, 3, 4, 5, 6, 7, 0, 1, 62);
        RSHA256_EXP(1, 2, 3, 4, 5, 6, 7, 0, 63);

        ctx->h[0] += wv[0];
        ctx->h[1] += wv[1];
        ctx->h[2] += wv[2];
        ctx->h[3] += wv[3];
        ctx->h[4] += wv[4];
        ctx->h[5] += wv[5];
        ctx->h[6] += wv[6];
        ctx->h[7] += wv[7];
    }
}

void rsha256_init(T ctx)
{
    rassert(ctx);

    ctx->h[0]    = rsha256_h0[0];
    ctx->h[1]    = rsha256_h0[1];
    ctx->h[2]    = rsha256_h0[2];
    ctx->h[3]    = rsha256_h0[3];
    ctx->h[4]    = rsha256_h0[4];
    ctx->h[5]    = rsha256_h0[5];
    ctx->h[6]    = rsha256_h0[6];
    ctx->h[7]    = rsha256_h0[7];
    ctx->len     = 0;
    ctx->tot_len = 0;
}

void rsha256_update(T ctx, unsigned char const *message, unsigned int len)
{
    rassert(ctx);
    rassert(message);

    unsigned             blk_nb;
    unsigned             new_len, rem_len, tmp_len;
    const unsigned char *shifted_message;

    tmp_len = RSHA256_BLOCK_SIZE - ctx->len;
    rem_len = len < tmp_len ? len : tmp_len;

    memcpy(&ctx->block[ctx->len], message, rem_len);

    if (ctx->len + len < RSHA256_BLOCK_SIZE) {
        ctx->len += len;
        return;
    }

    new_len = len - rem_len;
    blk_nb  = new_len / RSHA256_BLOCK_SIZE;

    shifted_message = message + rem_len;

    rsha256_transf(ctx, ctx->block, 1);
    rsha256_transf(ctx, shifted_message, blk_nb);

    rem_len = new_len % RSHA256_BLOCK_SIZE;

    memcpy(ctx->block, &shifted_message[blk_nb << 6], rem_len);

    ctx->len = rem_len;
    ctx->tot_len += (blk_nb + 1) << 6;
}

void rsha256_final(T ctx, unsigned char *digest)
{
    rassert(ctx);
    rassert(digest);

    unsigned blk_nb;
    unsigned pm_len;
    unsigned len_b;

    blk_nb = (1 + ((RSHA256_BLOCK_SIZE - 9) < (ctx->len % RSHA256_BLOCK_SIZE)));

    len_b  = (ctx->tot_len + ctx->len) << 3;
    pm_len = blk_nb << 6;

    memset(ctx->block + ctx->len, 0, pm_len - ctx->len);
    ctx->block[ctx->len] = 0x80;
    UNPACK32(len_b, ctx->block + pm_len - 4);

    rsha256_transf(ctx, ctx->block, blk_nb);

    UNPACK32(ctx->h[0], &digest[0]);
    UNPACK32(ctx->h[1], &digest[4]);
    UNPACK32(ctx->h[2], &digest[8]);
    UNPACK32(ctx->h[3], &digest[12]);
    UNPACK32(ctx->h[4], &digest[16]);
    UNPACK32(ctx->h[5], &digest[20]);
    UNPACK32(ctx->h[6], &digest[24]);
    UNPACK32(ctx->h[7], &digest[28]);
}

void rsha256(unsigned char const *message, unsigned int len,
             unsigned char *digest)
{
    struct T ctx;
    rsha256_init(&ctx);
    rsha256_update(&ctx, message, len);
    rsha256_final(&ctx, digest);
}

