#include "rrandom.h"

#include <stdlib.h>

#include "rassert.h"

double rrandom_uniform_zero2one() { return (double)rand() / RAND_MAX; }

double rrandom_uniform_real(double s, double e)
{
    double rate = (double)rand() / RAND_MAX;
    return s + (e - s) * rate;
}

int rrandom_uniform_int(int s, int e)
{
    int base = rand();
    if (base == RAND_MAX) return rrandom_uniform_int(s, e);
    int range     = e - s;
    int remainder = RAND_MAX % range;
    int bucket    = RAND_MAX / range;
    if (base < RAND_MAX - remainder)
        return s + base / bucket;
    else
        return rrandom_uniform_int(s, e);
}

int32_t rrandom_rand32() { return ((rand() << 16) + (rand() << 1) + (rand() & 1)); }

int32_t rrandom_uniform_int32(int32_t s, int32_t e)
{
    rassert(s < e);
    int32_t base = rrandom_rand32();
    if (base == RAND32_MAX) return rrandom_uniform_int32(s, e);
    int range     = e - s;
    int remainder = RAND32_MAX % range;
    int bucket    = RAND32_MAX / range;
    if (base < RAND32_MAX - remainder)
        return s + base / bucket;
    else
        return rrandom_uniform_int32(s, e);
}
