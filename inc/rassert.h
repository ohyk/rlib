#ifdef NDEBUG
#define rassert(e) ((void)0)
#else 
#include "rexcept.h"
#ifdef __cplusplus
extern "C"
{
#endif
extern RLIB_EXPORT_FLAG void rassert(int e);
#define rassert(e) ((void)((e) || (RAISE(RAssertFailed), 0)))
#ifdef __cplusplus
}
#endif
#endif
