#include "rtree.h"

#include "rassert.h"
#include "rarith.h"
#include "rlog.h"
#include "rmem.h"

typedef struct TreeNode_T *TreeNode_T;
struct TreeNode_T
{
    long        height;
    void       *val;
    TreeNode_T left;
    TreeNode_T right;
};

#define T RTree_T
struct T
{
    int (*cmp)(void const *, void const *);
    TreeNode_T root;
};

static int height(TreeNode_T n) { return (n) ? n->height : -1; }

static TreeNode_T single_rotation_with_left(TreeNode_T k1)
{
    TreeNode_T k2 = k1->left;
    k1->left       = k2->right;
    k2->right      = k1;

    k1->height = RMAX(height(k1->left), height(k1->right)) + 1;
    k2->height = RMAX(height(k2->left), height(k2->right)) + 1;

    return k2;  // new root
}

static TreeNode_T single_rotation_with_right(TreeNode_T k1)
{
    TreeNode_T k2 = k1->right;
    k1->right      = k2->left;
    k2->left       = k1;

    k1->height = RMAX(height(k1->left), height(k1->right)) + 1;
    k2->height = RMAX(height(k2->left), height(k2->right)) + 1;

    return k2;  // new root
}

static TreeNode_T double_rotation_with_left(TreeNode_T k1)
{
    k1->left = single_rotation_with_right(k1->left);
    return single_rotation_with_left(k1);
}

static TreeNode_T double_rotation_with_right(TreeNode_T k1)
{
    k1->right = single_rotation_with_left(k1->right);
    return single_rotation_with_right(k1);
}

T rtree_new(int (*cmp)(void const *, void const *))
{
    T t     = ALLOC(sizeof(*t));
    t->cmp  = cmp;
    t->root = NULL;
    return t;
}

static inline void *get_val(TreeNode_T n) { return (n) ? n->val : NULL; }

static TreeNode_T insert(TreeNode_T n, void *v, int (*cmp)(void const *, void const *))
{
    if (!n) {
        NEW(n);
        n->val  = v;
        n->left = n->right = NULL;
        n->height          = 0;
    } else if (cmp(v, n->val) < 0) {  // go left
        n->left = insert(n->left, v, cmp);
        if (height(n->left) - height(n->right) == 2) {
            if (cmp(v, n->left->val) < 0)
                n = single_rotation_with_left(n);  // LL
            else
                n = double_rotation_with_left(n);  // LR
        }
    } else if (cmp(v, n->val) > 0) {  // go right
        n->right = insert(n->right, v, cmp);
        if (height(n->right) - height(n->left) == 2) {
            if (cmp(v, n->right->val) > 0)
                n = single_rotation_with_right(n);  // RR
            else
                n = double_rotation_with_right(n);  // RL
        }
    }

    n->height = RMAX(height(n->left), height(n->right)) + 1;
    return n;
}

void rtree_insert(T t, void *v)
{
    rassert(t);
    t->root = insert(t->root, v, t->cmp);
}

static TreeNode_T find(TreeNode_T n, void const *k,
                        int (*cmp)(void const *, void const *))
{
    if (!n)
        return NULL;
    else if (cmp(k, n->val) > 0)
        return find(n->right, k, cmp);
    else if (cmp(k, n->val) < 0)
        return find(n->left, k, cmp);
    else
        return n;
}

void *rtree_find(T t, void const *k)
{
    rassert(t);
    rassert(k);
    return get_val(find(t->root, k, t->cmp));
}

static TreeNode_T find_max(TreeNode_T n)
{
    if (!n)
        return NULL;
    else if (!n->right)
        return n;
    else
        return find_max(n->right);
}

void *rtree_find_max(T t)
{
    rassert(t);
    return get_val(find_max(t->root));
}

static TreeNode_T find_min(TreeNode_T n)
{
    if (!n)
        return NULL;
    else if (!n->left)
        return n;
    else
        return find_min(n->left);
}

void *rtree_find_min(T t)
{
    rassert(t);
    return get_val(find_min(t->root));
}

static void map(TreeNode_T n, void (*app)(void *, void *), void *cl)
{
    if (!n) return;
    map(n->left, app, cl);
    app(n->val, cl);
    map(n->right, app, cl);
}

void rtree_map(T t, void (*apply)(void *v, void *cl), void *cl)
{
    rassert(t);
    rassert(apply);
    map(t->root, apply, cl);
}
