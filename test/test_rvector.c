/* #define RVECTOR_MEM */
#include <stdio.h>
#include <string.h>

#include "rmem.h"
#include "rarena.h"
#include "rlog.h"
#include "rvector.h"

#ifdef RVECTOR_MEM
static RArena_T StaticArena;
static inline RArena_T get_sa(void)
{
    if (!StaticArena) {
        StaticArena = rarena_new();
    }
    return StaticArena;
}

static void *aalloc(size_t nbs, char const *f, int l)
{
    return rarena_alloc(get_sa(), nbs, f, l);
}

static void afree(void *p, char const *f, int l)
{
}

static struct RVectorHooks_T SaHooks = { aalloc, afree, NULL };

#endif

void print_iar(int *iar, int len, FILE *fp)
{
    for (int i = 0; i < len; ++i)
        fprintf(fp, "%d ", iar[i]);
    fprintf(fp, "\n");
}

int main(int argc, char *argv[])
{
    int *iar = NULL;
    int i;
#ifdef RVECTOR_MEM
    RVector_T buf = rvector_new(sizeof (int), &SaHooks);
#else
    RVector_T buf = rvector_new(sizeof (int), NULL);
    /* Rvector_T buf = rvector_new(sizeof (int)); */
#endif

    FILE *fp;
    if ((fp = fopen("t2.txt", "w+")) == NULL)
        rlog_exit("open t2.txt error");

    for (i = 0; i < 233; i++)
        rvector_append(buf, &i);

    iar = rvector_get(buf);

    fprintf(fp, "iar(%p): \n", iar);
    for (i = 0; i < 233; i++)
        fprintf(fp, "%d ", iar[i]);
    fprintf(fp, "\n");

    iar = rvector_get_dispose(&buf);

    fprintf(fp, "iar(%p): \n", iar);
    for (i = 0; i < 233; i++)
        fprintf(fp, "%d ", iar[i]);
    fprintf(fp, "\n");
    
    fprintf(fp, "buf: %p\n", buf);

#ifdef RVECTOR_MEM
    iar = 0;
#else
    FREE(iar);
#endif
    fprintf(fp, "iar: %p\n", iar);

#ifdef MEM_CHK
    rmem_print_allocated();
    rmem_print_freed();
#endif

    fprintf(fp, "---\n\n");

    long *lar = NULL;
    long l;
#ifdef RVECTOR_MEM
    buf = rvector_new(sizeof (long), &SaHooks);
#else
    buf = rvector_new(sizeof (long), NULL);
    /* buf = rvector_new(sizeof (long)); */
#endif

    for (l = 0; l < 333; l++)
        rvector_append(buf, &l);

    lar = rvector_get(buf);

    fprintf(fp, "lar(%p) use len(%zd): \n", lar, rvector_length(buf));
    for (i = 0; i < 333; i++)
        fprintf(fp, "%ld ", lar[i]);
    fprintf(fp, "\n");

    iar = rvector_get_dispose(&buf);

    fprintf(fp, "lar(%p): \n", lar);
    for (i = 0; i < 333; i++)
        fprintf(fp, "%ld ", lar[i]);
    fprintf(fp, "\n");
    
    fprintf(fp, "buf: %p\n", buf);

#ifdef RVECTOR_MEM
    iar = 0;
#else
    FREE(iar);
#endif
    fprintf(fp, "iar: %p\n", iar);

#ifdef MEM_CHK
    rmem_print_allocated();
    rmem_print_freed();
#endif

    fprintf(fp, "---\n\n");

#ifdef RVECTOR_MEM
    buf = rvector_new(sizeof (*iar), &SaHooks);
#else
    buf = rvector_new(sizeof (*iar), NULL);
    /* buf = rvector_new(sizeof (*iar)); */
#endif
    for (i = 0; i < 220; i++)
        rvector_append(buf, &i);
    
    iar = rvector_get(buf);
    fprintf(fp, "iar(%p) use len(%zd): \n", iar, rvector_length(buf));
    for (i = 0; i < 220; i++)
        fprintf(fp, "%d ", iar[i]);
    fprintf(fp, "\n");

    fprintf(fp, "subtract: %d\n", *(int *) rvector_subtract(buf));
    long ilen = rvector_length(buf);
    fprintf(fp, "subtract after use length: %ld\n", ilen);

    fprintf(fp, "subtract: %d\n", *(int *) rvector_subtract(buf));
    ilen = rvector_length(buf);
    fprintf(fp, "subtract after use length: %ld\n", ilen);

    for (ilen = rvector_length(buf); ilen > 0; ilen = rvector_length(buf))
        fprintf(fp, "before length: %ld --- subtract: %d\n", ilen, *(int *) rvector_subtract(buf));

    iar = rvector_get_dispose(&buf);

    fprintf(fp, "iar(%p): \n", iar);
    if (ilen > 0) {
        for (i = 0; i < ilen; i++)
            fprintf(fp, "%d ", iar[i]);
        fprintf(fp, "\n");
    }

#ifdef RVECTOR_MEM
    iar = 0;
#else
    FREE(iar);
#endif
    fprintf(fp, "iar: %p\n", iar);

#ifdef MEM_CHK
    rmem_print_allocated();
    rmem_print_freed();
#endif

    fprintf(fp, "\n=== test rvector_reset ===\n");

#ifdef RVECTOR_MEM
    buf = rvector_new(sizeof (int), &SaHooks);
#else
    buf = rvector_new(sizeof (int), NULL);
    /* buf = rvector_new(sizeof (int)); */
#endif

#ifdef RVDEBUG
    rvector_print_info(buf);
#endif

    fprintf(fp, "add 10 element to buf...\n");
    for (i = 0; i < 10; ++i)
        rvector_append(buf, &i);
    iar = rvector_get(buf);
    fprintf(fp, "buf vals: ");
    print_iar(iar, 10, fp);
    fprintf(fp, "buf use length: %zd\n", rvector_length(buf));
    fprintf(fp, "buf capicaty: %zd\n", rvector_capacity(buf));

#ifdef RVDEBUG
    rvector_print_info(buf);
#endif

#ifdef MEM_CHK
    rmem_print_allocated();
    rmem_print_freed();
#endif

    fprintf(fp, "buf reset...\n");
    rvector_reset(buf);
    fprintf(fp, "buf use length: %zd\n", rvector_length(buf));
    fprintf(fp, "buf capicaty: %zd\n", rvector_capacity(buf));

    fprintf(fp, "buf reserve...\n");
    rvector_reverse(buf, 256);
    fprintf(fp, "buf use length: %zd\n", rvector_length(buf));
    fprintf(fp, "buf capicaty: %zd\n", rvector_capacity(buf));
    fprintf(fp, "buf size: %zd\n", rvector_size(buf));

    i = 999;
    rvector_append(buf, &i);

    fprintf(fp, "buf use length: %zd\n", rvector_length(buf));

    iar = rvector_get_dispose_shrink(&buf);

    fprintf(fp, "buf vals: ");
    print_iar(iar, 10, fp);

    /* rvector_get_dispose(&buf); */
#ifdef RVECTOR_MEM
    iar = 0;
#else
    /* FREE(iar); */
#endif
#ifdef MEM_CHK
    rmem_print_allocated();
    rmem_print_freed();
#endif

    return 0;
}
