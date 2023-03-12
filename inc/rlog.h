#ifndef RLOG_H
#define RLOG_H
#include "rdef.h"
#include "rexcept.h"

#ifdef __GNUC__
/*========================================================================*/
/*
 * This macro stops 'gcc -Wall' complaining that 'control reaches end of 
 * non-void function" if we use following functions to terminate main()
 * or some other non-void function.
 */
/*========================================================================*/
#define NORETURN __attribute__ ((__noreturn__))
#else 
#define NORETURN
#endif /* __GNUC__ */

#ifdef __cplusplus
extern "C" {
#endif

extern RLIB_EXPORT_FLAG RExcept_T const RLogFileError;

extern RLIB_EXPORT_FLAG Bool_T rlog_reset(char const *log_file);

extern RLIB_EXPORT_FLAG void rlog_message(char const *format, ...);
extern RLIB_EXPORT_FLAG void rlog_error_message(char const *format, ...);
extern RLIB_EXPORT_FLAG void rlog_exit(char const *format, ...) NORETURN;
extern RLIB_EXPORT_FLAG void rlog__exit(char const *format, ...) NORETURN;
extern RLIB_EXPORT_FLAG void rlog_exit_code(int ecode, char const *format,
                                            ...) NORETURN;
extern RLIB_EXPORT_FLAG void rlog_fatal(char const *format, ...) NORETURN;
extern RLIB_EXPORT_FLAG void rlog_usage_error(char const *format, ...) NORETURN;
extern RLIB_EXPORT_FLAG void rlog_cli_error(char const *format, ...) NORETURN;

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: RLOG_H */
