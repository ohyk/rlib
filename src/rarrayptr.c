#include "rarrayptr.h"

#include "rarray.h"
#include "rassert.h"
#include "rmem.h"

#define T RArray_T

T rarrayptr_new(int length) { return rarray_new(length, sizeof(void *)); }

void rarrayptr_free(T *array) { rarray_free(array); }

void *rarrayptr_get(T array, int i) { return rarray_get(array, i); }

void *rarrayptr_put(T array, int i, void *eptr) { return rarray_put(array, i, eptr); }

int rarrayptr_length(T array) { return rarray_length(array); }

T rarrayptr_copy(T array, int length) { return rarray_copy(array, length); }

void rarrayptr_resize(T array, int length) { return rarray_resize(array, length); }
