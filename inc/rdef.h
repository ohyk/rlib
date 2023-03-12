/*====================================================================*/
/*
 * Copyright (C) 2018,2021 Richard Guo - aka.yunkai@gmail.com
 * Created on 2018/02/23
 * 
 * Declare global variable
 */
/*====================================================================*/
#ifndef RDEF_H
#define RDEF_H 

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

#ifndef RLIB_HAS_BOOL_TYPE
#define RLIB_HAS_BOOL_TYPE
typedef enum { False, True } Bool_T;
#endif /* end of include guard: RLIB_HAS_BOOL_TYPE */

#ifndef EOF
#define EOF (-1)
#endif /* ifndef EOF */

/*====================================================================*/
/*
 * winNT dll export func
 */
/*====================================================================*/

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
   //define something for Windows (32-bit and 64-bit, this part is common)
#define RLIB_EXPORT_FLAG __declspec(dllexport)
#include <sys/types.h>
   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #else
      //define something for Windows (32-bit only)
   #endif
#elif __APPLE__
#define RLIB_HAS_EXECINFO
    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR
         // iOS Simulator 
    #endif
    #if TARGET_OS_MACCATALYST
         // Mac's Catalyst (ports iOS API into Mac, like UIKit).
    #elif TARGET_OS_IPHONE
        // iOS device
    #elif TARGET_OS_MAC
        // Other kinds of Mac OS
    #else
    #   error "Unknown Apple platform"
    #endif
#elif __linux__
#define RLIB_HAS_EXECINFO
    // linux
#elif __unix__ // all unices not caught above
#define RLIB_HAS_EXECINFO
    // Unix
#elif defined(_POSIX_VERSION)
#define RLIB_HAS_EXECINFO
    // POSIX
#else
#   error "Unknown compiler"
#endif

#ifndef RLIB_EXPORT_FLAG
#define RLIB_EXPORT_FLAG
#endif

#ifdef __GNUC__
#define RLIB_NO_RETURN __attribute__ ((__noreturn__))
#else
#define RLIB_NO_RETURN
#endif

/*====================================================================*/
/*
 * arena.c/mem.c/memchk.c once allocate maximum
 */
/*====================================================================*/
#ifndef MEM_MAXIMUM
#define MEM_MAXIMUM LONG_MAX
#endif

#endif /* ifndef RDEF_H */
