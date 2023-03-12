#ifndef RJSON_H
#define RJSON_H
#include <stdio.h>

#include "ratom.h"
#include "rdef.h"

typedef enum Json_Type_T {
    Error,
    Nil,
    Digit,
    String,
    Boolean,
    Array,
    Object
} Json_Type_T;

typedef int *Boolean_T;

typedef struct Json_T
{
    Json_Type_T type;
    void       *val;
} *Json_T;

#ifdef __cplusplus
extern "C"
{
#endif

Json_T json_parse(FILE *fp);
// void json_print(void *x, int depth);
char *json_print(void *x, int depth);

Json_T json_object_new(int hint, int cmp(void const *x, void const *y),
                       unsigned hash(void const *x));
Json_T json_object_get(Json_T jt, void const *key);
Json_T json_object_put(Json_T jt, void const *key, Json_T val);
int    json_object_length(Json_T jt);
void json_object_map(Json_T jt, void app(void const *, void **, void *cl), void *cl);

Json_T json_array_new(int hint);
Json_T json_array_get(Json_T jt, int i);
Json_T json_array_put(Json_T jt, int i, Json_T val);
int    json_array_length(Json_T jt);
Json_T json_array_addlo(Json_T array, Json_T jt);
Json_T json_array_addhi(Json_T array, Json_T jt);
Json_T json_array_remlo(Json_T array);
Json_T json_array_remhi(Json_T array);

Json_T json_boolean_new(int b);
Json_T json_null_new(void);
Json_T json_digit_new(double d);
Json_T json_string_new(char *str);

void json_dispose(Json_T *jt);

#ifdef __cplusplus
}
#endif

#endif /* ifndef RJSON_H */
