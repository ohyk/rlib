/*!	TODO(rich): bug: "" key
 *	\todo bug: "" key
 */
#include <stddef.h>
#include <string.h>
#include "assert.h"
#include "mem.h"
#include "table.h"

#define T Table_T
#define RMAX(x, y) ((x) > (y) ? (x) : (y))
#define RMIN(x, y) ((x) < (y) ? (x) : (y))

struct T {
    int length;
    int timestamp;
    int (*cmp)(void const *x, void const *y);
    unsigned (*hash)(void const *key);
    struct binding {
        int height;
        unsigned hash_key;
        void *value;
        void const *key;
        struct binding *left;
        struct binding *right;
    } *tree;
};

/* static functions */
static int height(struct binding *t)
{
    return t ? t->height : -1;
}

static struct binding *single_rotation_with_left(struct binding *k1)
{
    struct binding *k2 = k1->left;
    k1->left = k2->right;
    k2->right = k1;

    k1->height = RMAX(height(k1->left), height(k1->right)) + 1;
    k2->height = RMAX(height(k2->left), height(k2->right)) + 1;
    return k2; /* new root */
}

static struct binding *single_rotation_with_right(struct binding *k1)
{
    struct binding *k2 = k1->right;
    k1->right = k2->left;
    k2->left = k1;

    k1->height = RMAX(height(k1->left), height(k1->right)) + 1;
    k2->height = RMAX(height(k2->left), height(k2->right)) + 1;
    return k2;
}

static struct binding *double_rotation_with_left(struct binding *k1)
{
    k1->left = single_rotation_with_right(k1->left);
    return single_rotation_with_left(k1);
}

static struct binding *double_rotation_with_right(struct binding *k1)
{
    k1->right = single_rotation_with_left(k1->right);
    return single_rotation_with_right(k1);
}

static int cmp_key(void const *x, void const *y)
{
    int a, b;
    a = strlen((char *)x);
    b = strlen((char *)y);
    return memcmp(x, y, RMIN(a, b) + 1);
}

static unsigned hash_key_atom(void const *x)
{
    return (unsigned long) x >> 2;
}

/* static int cmp_hash_key(void const *x, void const *y) */
/* { */
    /* if (*(unsigned *)x > *(unsigned *) y) { */
        /* return 1; */
    /* } else if (*(unsigned *)x < *(unsigned *) y) { */
        /* return -1; */
    /* } else  */
        /* return 0; */
/* } */

/* functions */
T table_new(int hint, 
        int cmp(void const *x, void const *y), 
        unsigned hash(void const *key))
{
    T tab;

    assert(hint >= 0);

    tab = ALLOC(sizeof (*tab));
    tab->length = 0;
    tab->timestamp = 0;
    tab->cmp    = cmp ? cmp : cmp_key;
    tab->hash   = hash ? hash : hash_key_atom;
    /* tab->cmp    = cmp ? cmp : cmp_hash_key; */
    tab->tree   = NULL;
    return tab;
}

int table_length(T table)
{
    assert(table);
    return table->length;
}

static struct binding *tree_find(struct binding *t, void const *key, 
        int cmp(void const *, void const *))
{
    if (!t)
        return NULL;
    /* else if (cmp(key, &t->hash_key) > 0) */
    else if (cmp(key, t->key) > 0)
        return tree_find(t->right, key, cmp);
    /* else if (cmp(key, &t->hash_key) < 0)  */
    else if (cmp(key, t->key) < 0) 
        return tree_find(t->left, key, cmp);
    else 
        return t;
}

void *table_get(T table, void const *key)
{
    assert(table);
    assert(key);
    /* unsigned hk = hash_key(key); */
    /* struct binding *p = tree_find(table->tree, &hk, table->cmp); */
    struct binding *p = tree_find(table->tree, key, table->cmp);
    return p ? p->value : NULL;
}

static struct binding *tree_insert(struct binding *t, unsigned *hk, void const *key, void *val,
        void **prev,
        int cmp(void const *, void const *))
{
    if (!t) {
        t = ALLOC(sizeof (*t));
        t->hash_key = *hk;
        t->value = val;
        t->key = key;
        t->height = 0;
        t->left = t->right = NULL;
        *prev = NULL;
    } else if (cmp(key, t->key) > 0) { /* go right */
        t->right = tree_insert(t->right, hk, key, val, prev, cmp);
        if (height(t->right) - height(t->left) == 2) {
            if (cmp(key, t->key) > 0)
                t = single_rotation_with_right(t);
            else 
                t = double_rotation_with_right(t);
        }
    } else if (cmp(key, t->key) < 0) { /* go right */
        t->left = tree_insert(t->left, hk, key, val, prev, cmp);
        if (height(t->left) - height(t->right) == 2) {
            if (cmp(key, t->key) < 0)
                t = single_rotation_with_left(t);
            else 
                t = double_rotation_with_left(t);
        }
    } else {
        /* exits */
        *prev = t->value;
        t->value = val;
    }

    t->height = RMAX(height(t->left), height(t->right)) + 1;
    return t;
}

void *table_put(T table, void const *key, void *value)
{
    assert(table);
    assert(key);

    void *prev;
    unsigned hk = (*table->hash)(key);
    table->tree = tree_insert(table->tree, &hk, key, value, &prev, table->cmp);
    if (prev == NULL)
        table->length++;
    table->timestamp++;
    return prev;
}

static void tree_map(struct binding *t, void apply(void const *key, void **val, void *cl), 
        void *cl)
{
    if (!t)
        return;
    tree_map(t->left, apply, cl);
    apply(t->key, &t->value, cl);
    tree_map(t->right, apply, cl);
}

void table_map(T table, void apply(void const *key, void **value, void *cl), 
        void *cl)
{
    assert(table);
    assert(apply);
    tree_map(table->tree, apply, cl);
}

static void **ars;
static void app_set_array(void const *key, void **val, void *cl)
{
    *ars++ = (void *) key;
    *ars++ = *val;
}

void **table_to_array(T table, void *end)
{
    assert(table);
    void **ar = ALLOC((2*table->length + 1) * sizeof (*ar));
    ars = ar;
    tree_map(table->tree, app_set_array, NULL);
    ars = end;
    ars = NULL;
    return ar;
}

static void tree_free(struct binding *t)
{
    if (!t)
        return;
    tree_free(t->left);
    tree_free(t->right);
    FREE(t);
}

void table_free(T *table)
{
    assert(table && *table);
    tree_free((*table)->tree);
    FREE(*table);
}
