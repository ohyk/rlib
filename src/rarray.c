#include "rarray.h"

#include <stdlib.h>
#include <string.h>

#include "rarrayrep.h"
#include "rassert.h"
#include "rmem.h"

#define T RArray_T

/* functions */

T rarray_new(int length, int size)
{
    T array;

    NEW(array);
    if (length > 0)
        rarrayrep_init(array, length, size, CALLOC(length, size));
    else
        rarrayrep_init(array, length, size, NULL);
    return array;
}

void rarrayrep_init(T array, int length, int size, void *ary)
{
    rassert(array);
    rassert((ary && length > 0) || (length == 0 && ary == NULL));
    rassert(size > 0);
    array->length = length;
    array->size   = size;
    if (length > 0)
        array->array = ary;
    else
        array->array = NULL;
}

void rarray_free(T *array)
{
    rassert(array && *array);
    FREE((*array)->array);
    FREE(*array);
}

int rarray_length(T array)
{
    rassert(array);
    return array->length;
}

int rarray_size(T array)
{
    rassert(array);
    return array->size;
}

void *rarray_get(T array, int i)
{
    rassert(array);
    rassert(i >= 0 && i < array->length);
    return array->array + i * array->size;
}

void *rarray_put(T array, int i, void *elm)
{
    rassert(array);
    rassert(i >= 0 && i < array->length);
    rassert(elm);
    memcpy(array->array + i * array->size, elm, array->size);
    return elm;
}

void rarray_resize(T array, int length)
{
    rassert(array);
    rassert(length >= 0);
    if (length == 0)
        FREE(array->array);
    else if (array->length == 0)
        array->array = ALLOC(length * array->size);
    else
        RESIZE(array->array, length * array->size);
    array->length = length;
}

T rarray_copy(T array, int length)
{
    T copy;

    rassert(array);
    rassert(length >= 0);

    copy = rarray_new(length, array->size);
    if (copy->length >= array->length && array->length > 0)
        memcpy(copy->array, array->array, array->length * array->size);
    else if (array->length > copy->length && copy->length > 0)
        memcpy(copy->array, array->array, copy->length * array->size);
    return copy;
}

void rarray_reshape(T array, int length, int size)
{
    rassert(array);
    rassert(length >= 0 && size > 0);

    if (length == 0) FREE(array->array);

    if (size != array->size) {
        if (array->length == 0)
            array->array = CALLOC(length, size);
        else {
            int i, n, sn;
            char *new = CALLOC(length, size);
            n         = array->length > length ? length : array->length;
            sn        = array->size > size ? size : array->size;
            for (i = 0; i < n; i++) {
                memcpy(new + (i * size), array->array + (i * array->size), sn);
            }
            FREE(array->array);
            array->array = new;
        }
    } else
        rarray_resize(array, length);
    array->size   = size;
    array->length = length;
}
