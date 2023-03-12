#include <stddef.h>
/* #include "assert.h" */
#include "mem.h"

#define Infinity (NULL)
#define T Skplist_T
typedef struct T *T;

struct T {
    void *element;
    int (*cmp)(void const *, void const *);
    T right;
    T down;
};

static T Bottom = NULL; /* Needs initialization */
static T Tail = NULL;

static int cmpatom(void const *x, void const *y)
{
    return x != y;
}

/* Initialization procedure */

T skplist_initialize(int cmp(void const *, void const *))
{
    T l;

    if (Bottom == NULL) {
        NEW(Bottom);
        Bottom->right = Bottom->down = Bottom;
        
        NEW(Tail);
        Tail->element = (void *) Infinity;
        Tail->right = Tail;
    }

    /* create the header node */
    NEW(l);
    l->element = (void *) Infinity;
    l->right = Tail;
    l->down = Bottom;
    return l;
}

/* return position of node containing Item, */
/* or Bottom if not found */
T skplist_find(T l, void *x)
{
    T current = l;

    Bottom->element = x;
    while (x != current->element) {
        if (x < current->element)
            current = current->down;
        else 
            current = current->right;
    }
    return current;
}

T skplist_insert(T l, void *x)
{
    T current = l;
    T newp;

    Bottom->element = x;
    while (current != Bottom) {
        while (x > current->element)
            current = current->right;
        
        /* if gap size is 3 or at bottom level */
        /* and must insert, then promote the middle element */
        if (current->element > current->down->right->right->element) {
            NEW(newp);
            newp->right = current->right;
            newp->down = current->down->right->right;
            current->right = newp;
            newp->element = current->element;
            current->element = current->down->right->element;
        } else 
            current = current->down;
    }

    /* Raise height of DSL if necessary  */
    if (l->right != Tail) {
        NEW(newp);
        newp->down = l;
        newp->right = Tail;
        newp->element = (void *) Infinity;
        l = newp;
    }
}
