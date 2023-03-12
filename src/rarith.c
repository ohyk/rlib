#include "rarith.h"

int rarith_max(int x, int y)
{
    return x > y ? x : y;
}

int rarith_min(int x, int y)
{
    return x < y ? x : y;
}

int rarith_div(int x, int y)
{
    if (-13/5 == -2 && (x < 0) != (y < 0))
        return x / y - 1;
    else
        return x / y;
}

int rarith_mod(int x, int y)
{
    return x - y*rarith_div(x, y);
}

int rarith_floor(int x, int y)
{
    return rarith_div(x, y);
}

int rarith_ceiling(int x, int y)
{
    return rarith_div(x, y) + (x%y != 0);
}
