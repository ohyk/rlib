#ifndef RARITH_H
#define RARITH_H
#include "rdef.h"

#ifndef RMAX
#define RMAX(x, y) ((x) > (y) ? (x) : (y))
#endif /* ifndef RMAX */
#ifndef RMIN
#define RMIN(x, y) ((x) < (y) ? (x) : (y))
#endif /* ifndef RMIN */

#ifdef __cplusplus
extern "C" {
#endif

extern RLIB_EXPORT_FLAG int rarith_max(int x, int y);
extern RLIB_EXPORT_FLAG int rarith_min(int x, int y);
extern RLIB_EXPORT_FLAG int rarith_div(int x, int y);
extern RLIB_EXPORT_FLAG int rarith_mod(int x, int y);
extern RLIB_EXPORT_FLAG int rarith_ceiling(int x, int y);
extern RLIB_EXPORT_FLAG int rarith_floor(int x, int y);

#ifdef __cplusplus
}
#endif

#endif
