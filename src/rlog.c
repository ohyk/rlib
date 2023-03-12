#include "rlog.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <io.h>
#else
#include <unistd.h>
#endif

#include "rassert.h"
#include "ratom.h"
#include "rexcept.h"
#include "rmem.h"

#define T RLog_T
typedef struct T *T;
struct T
{
    char const *log_name;
    FILE       *log;
};

T rlog_new(char const *file, FILE *fp)
{
    T log         = ALLOC(sizeof(*log));
    log->log      = fp;
    log->log_name = file;
    return log;
}

static T          DefaultLog;
static char const StandardErrorString[] = "Standard Error";
RExcept_T const   RLogFileError         = {"Rlog log file failed"};

static T get_default_log(void)
{
    if (!DefaultLog) {
        DefaultLog = rlog_new(StandardErrorString, stderr);
    }
    return DefaultLog;
}

Bool_T rlog_reset(char const *log_file)
{
    T log = get_default_log();
    if (!log_file) return False;
    if (log->log_name != StandardErrorString) {  // log is stderr don't close
        if (fclose(log->log) == -1) {
            RAISE(RLogFileError);
        }
    }
    log->log_name = ratom_string(log_file);  // set file path
    if ((log->log = fopen(log_file, "a+")) == NULL) {
        // open failed reset log to stderr
        log->log      = stderr;
        log->log_name = StandardErrorString;

        RAISE(RLogFileError);
    }
    return True;
}

/* _Noreturn static void terminate(Bool_T u3) */
NORETURN static void terminate(Bool_T u3)
{
    char *s = getenv("EF_DUMPCORE");
    if (s != NULL && *s != '\0')
        abort();
    else if (u3)
        exit(EXIT_FAILURE);
    else
        _exit(EXIT_FAILURE);
}

#ifndef BUF_SIZE
#define BUF_SIZE 512
#endif
static void output_error(Bool_T use_err, int err, Bool_T flush_stdout, char const *format,
                         va_list ap)
{
    char buf[BUF_SIZE + BUF_SIZE + BUF_SIZE];
    char user_msg[BUF_SIZE];
    char err_text[BUF_SIZE];

    vsnprintf(user_msg, BUF_SIZE, format, ap);

    if (use_err)
        snprintf(err_text, BUF_SIZE, " [%s]", strerror(err));
    else
        *err_text = '\0';

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    snprintf(buf, BUF_SIZE + BUF_SIZE + BUF_SIZE, "[%lld]%s %s\n", (long long)time(NULL),
             err_text, user_msg);
#else
    snprintf(buf, BUF_SIZE + BUF_SIZE + BUF_SIZE, "[%d:%lld]%s %s\n", getpid(),
             (long long)time(NULL), err_text, user_msg);
#endif

    if (flush_stdout) fflush(stdout);  // flush any pending stdout
    fputs(buf, get_default_log()->log);
    fflush(get_default_log()->log);  // in case log is not line-buffered
}

void rlog_error_message(char const *format, ...)
{
    va_list args;
    int     scode;

    scode = errno;  // in case we change it here
    va_start(args, format);
    output_error(True, errno, True, format, args);
    va_end(args);

    errno = scode;
}

void rlog_message(char const *format, ...)
{
    va_list args;

    va_start(args, format);
    output_error(False, 0, True, format, args);
    va_end(args);
}

void rlog_exit(char const *format, ...)
{
    va_list args;

    va_start(args, format);
    output_error(True, errno, True, format, args);
    va_end(args);

    terminate(True);
}

void rlog__exit(char const *format, ...)
{
    va_list args;

    va_start(args, format);
    output_error(True, errno, False, format, args);
    va_end(args);

    terminate(False);
}

void rlog_exit_code(int ecode, char const *format, ...)
{
    va_list args;

    va_start(args, format);
    output_error(True, ecode, True, format, args);
    va_end(args);

    terminate(True);
}

void rlog_fatal(char const *format, ...)
{
    va_list args;

    va_start(args, format);
    output_error(False, 0, True, format, args);
    va_end(args);

    terminate(True);
}

void rlog_usage_error(char const *format, ...)
{
    va_list args;

    fflush(stdout);

    fprintf(stderr, "Usage: ");
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fflush(stderr);
    exit(EXIT_FAILURE);
}

void rlog_cli_error(char const *format, ...)
{
    va_list args;

    fflush(stdout);

    fprintf(stderr, "CLI usage error: ");
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fflush(stderr);
    exit(EXIT_FAILURE);
}
