#ifndef STACKREP_H
#define STACKREP_H 
#include "rdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define T Stack_T
typedef struct T *T;

/* stack representation */
struct T {
    int count;
    struct elem {
        void *x;
        struct elem *link;
    } *head;
};

/* the only way to init */
extern void stackrep_init(T stack);

#ifdef __cplusplus
}
#endif

#undef T 

#endif /* ifndef STACKREP_H */
