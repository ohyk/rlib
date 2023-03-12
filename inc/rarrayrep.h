#ifndef RARRAYREP_H
#define RARRAYREP_H
#include "rdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define T RArray_T

struct T
{
    int   length;
    int   size;
    char *array;
};

extern RLIB_EXPORT_FLAG void rarrayrep_init(T array, int length, int size, void *ary);

#ifdef __cplusplus
}
#endif

#undef T

#endif /* ifndef RARRAYREP_H */
