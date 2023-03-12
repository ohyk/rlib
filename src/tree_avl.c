#include <stdio.h>
#include <stddef.h>
#include "assert.h"
#include "mem.h"
#include "tree.h"

#define RMAX(x, y) (((x) > (y)) ? (x) : (y))
/* #define RMIN(x, y) (((x) < (y)) ? (x) : (y)) */

#define T Tree_T
struct T {
    int height;
    Tree_Element_t x;
    T left;
    T right;
};

static int height(T p)
{
    return (p) ? p->height : -1;
}

/* RR */
static T single_rotation_with_right(T k1)
{
    T k2 = k1->right;
    k1->right = k2->left;
    k2->left = k1;

    k1->height = RMAX(height(k1->left), height(k1->right)) + 1;
    k2->height = RMAX(height(k2->left), height(k2->right)) + 1;
    return k2;
}

/* LL */
static T single_rotation_with_left(T k1)
{
    T k2 = k1->left;
    k1->left = k2->right;
    k2->right = k1;

    k1->height = RMAX(height(k1->left), height(k1->right)) + 1;
    k2->height = RMAX(height(k2->left), height(k2->right)) + 1;
    return k2;
}

/* RL */
static T double_rotation_with_right(T k1)
{
    k1->right = single_rotation_with_left(k1->right);
    return single_rotation_with_right(k1);
}

/* LR */
static T double_rotation_with_left(T k1)
{
    k1->left = single_rotation_with_right(k1->left);
    return single_rotation_with_left(k1);
}

Tree_Element_t tree_retrieve(T t)
{
    assert(t);
    return t->x;
}

Tree_Element_t tree_find_min(T t)
{
    if (!t)
        return NULL;
    else if (!t->left)
        return t->x;
    else 
        return tree_find_min(t->left);
}

Tree_Element_t tree_find_max(T t)
{
    if (!t)
        return NULL;
    else if (!t->right)
        return t->x;
    else 
        return tree_find_max(t->right);
}

Tree_Element_t tree_find(void *n, int (*cmp)(void *, void *), T t)
{
    assert(cmp);

    if (!t)
        return NULL;
    else if (cmp(n, t->x) > 0)
        return tree_find(n, cmp, t->right);
    else if (cmp(n, t->x) < 0)
        return tree_find(n, cmp, t->left);
    else 
        return t->x;
}

T tree_insert(Tree_Element_t e, int (*cmp)(void *, void *), T t)
{
    assert(cmp);
    if (!t) {
        /* create new node */
        t = ALLOC(sizeof(*t));
        t->height = 0;
        t->x = e;
        t->left = t->right = NULL;
    } else if (cmp(e, t->x) < 0) { /* go left */
        t->left = tree_insert(e, cmp, t->left);
        if (height(t->left) - height(t->right) == 2) {
            if (cmp(e, t->left->x) < 0)
                t = single_rotation_with_left(t);   /* LL */
            else 
                t = double_rotation_with_left(t);   /* LR */
        }
    } else if (cmp(e, t->x) > 0) { /* go right */
        t->right = tree_insert(e, cmp, t->right);
        if (height(t->right) - height(t->left) == 2) {
            if (cmp(e, t->right->x) > 0)
                t = single_rotation_with_right(t);  /* RR */
            else 
                t = double_rotation_with_right(t);  /* RL */
        }
    }
    /* x is exits, we do nothing */

    t->height = RMAX(height(t->left), height(t->right)) + 1;
    return t;
}

void print_tree(void (*pt)(void *x), T t)
{
    if (!t || !pt)
        return;
    for (int i = 0; i < t->height; i++)
        putchar(' ');
    pt(t->x);
    print_tree(pt, t->left);
    print_tree(pt, t->right);
}

void print_left(void (*pt)(void *x), T t)
{
    if (!t || !pt)
        return;
    print_tree(pt, t->left);
}

void print_right(void (*pt)(void *x), T t)
{
    if (!t || !pt)
        return;
    print_tree(pt, t->right);
}
