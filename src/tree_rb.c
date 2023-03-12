#include <stddef.h>
#include "mem.h"
#include "tree.h"

#define T Tree_T

typedef enum Color_T { Red, Black } Color_T;

struct T {
    Tree_Element_t element;
    T left;
    T right;
    Color_T color;
};

T NullNode = NULL; /* heads initialization */

/* Initialization procedure */
T initialize(void)
{
    T t;
    if (NullNode == NULL) {
        NullNode = ALLOC(sizeof (*NullNode));
        NullNode->left = NullNode->right = NullNode;
        NullNode->color = Black;
        NullNode->element = infinity;
    }

    /* Create the header node */
    t = ALLOC(sizeof (*t));
    t->element = neg_infinity;
    t->left = t->right = NullNode;
    t->color = Black;

    return t;
}

/* Perform a rotation at node X */
/* (whose parent is passed as a parameter) */
/* The child is deduced by examining Item */
static T rotate(Tree_Element_t e, T parent)
{
    if (e < parent->element)
        return parent->left = e < parent->left->element ?
            single_rotation_with_left(parent->left) : 
            single_rotation_with_right(parent->left);
    else 
        return parent->right = e < parent->right->element ? 
            single_rotation_with_left(parent->right) : 
            single_rotation_with_right(parent->right);
}

static T X, P, GP, GGP;

static void handle_reorient(Tree_Element_t e, T t)
{
    X->color = Red; /* Do the color flip */
    X->left->color = Black;
    X->right->color = Black;

    if (P->color == Red) { /* Have to rotate */
        GP->color = Red;
        if (e < GP->element != e < P->element)
            P = rotate(e, GP); /* Start double rotation */
        X = rotate(e, GGP);
        X->color = Black;
    }
    t->right->color = Black; /* Make root black */
}

T insert(Tree_Element_t e, T t)
{
    X = P = GP = t;
    NullNode->element = e;

    while (X->element != e) { /* Descend down the tree */
        GGP = GP; GP = P; P = X;
        if (e < X->element)
            X = X->left;
        else 
            X = X->right;
        if (X->left->color == Red && X->right->color == Red)
            handle_reorient(e, t);
    }

    if (X != NullNode)
        return NullNode; /* Duplicate */

    X = ALLOC(sizeof (*X));
    X->element = e;
    X->left = X->right = NullNode;

    if (e < P->element) /* Attach to its parent */
        P->left = X;
    else 
        P->right = X;
    handle_reorient(e, t); /* Color red; maybe rotate */

    return t;
}

/* print the tree, watch out for NullNode, */
/* and skip header */
static void do_print(T t)
{
    if (t == NullNode) {
        do_print(t->left);
        output(t->element);
        do_print(t->right);
    }
}
