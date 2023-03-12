#ifndef RRANDOM_H
#define RRANDOM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rdef.h"

#ifndef RAND32_MAX
#define RAND32_MAX 0x7FFFFFFF
#endif

/*========================================================================*/
/*
 * Generate a random number in the range of [0, 1]
 */
/*========================================================================*/
extern RLIB_EXPORT_FLAG double rrandom_uniform_zero2one();

/*========================================================================*/
/*
 * Generate a random real number in [s, e]
 */
/*========================================================================*/
extern RLIB_EXPORT_FLAG double rrandom_uniform_real(double s, double e);

/*========================================================================*/
/*
 * Generate a random integer number in [s, e]
 *
 * Sometimes RAND_MAX just have 32767, so want generate large than this 
 * number must use rrandom_rand32() or rrandom_uniform_int32().
 */
/*========================================================================*/
extern RLIB_EXPORT_FLAG int rrandom_uniform_int(int s, int e);

/*========================================================================*/
/*
 * Generate a random integer number of 32 bits
 */
/*========================================================================*/
extern RLIB_EXPORT_FLAG int32_t rrandom_rand32();
extern RLIB_EXPORT_FLAG int32_t rrandom_uniform_int32(int32_t s, int32_t e);


#ifdef __cplusplus
}
#endif

#endif /* end of include guard: RRANDOM_H */
