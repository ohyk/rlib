#ifndef BUFFERIO_H
#define BUFFERIO_H 
#include "rdef.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define INPUT_BUF_SIZE  4096
#define BUFFER_EOF      (EOF)

typedef struct Input_Buffer_T
{
    char  buf[INPUT_BUF_SIZE];
    int   offset;
    int   count;
    FILE *in;
} Input_Buffer_T;

typedef struct
{
    char  *buf;
    size_t offset;
    size_t length;
    FILE  *out;
} Output_Buffer_T;

/* default global buffer */
extern RLIB_EXPORT_FLAG Input_Buffer_T  Global_Input_Buffer;
extern RLIB_EXPORT_FLAG Output_Buffer_T Global_Output_Buffer;

/* input */
/* default buffer */
extern int  buf_getchar(void);
extern void buf_ungetchar(int c);
extern long buf_getblock(long length, char *block);

extern int  buf_getchar_inbuffer(Input_Buffer_T *ibp);
extern void buf_ungetchar_inbuffer(Input_Buffer_T *ibp, int c);
extern long buf_getblock_inbuffer(Input_Buffer_T *ibp, long length, char *block);

/* output */
/* default buffer */
extern int buf_putchar(int c);
extern int buf_putblock(long length, char *byts);

extern int buf_unputchar(void);
extern int buf_unputblock(long length, char *byts);
extern int buf_unputchar_outbuffer(Output_Buffer_T *obp);
extern int buf_unputblock_outbuffer(Output_Buffer_T *obp, long length, char *byts);

extern void buf_cleanup_outbuffer(Output_Buffer_T *obp);

extern int buf_putchar_outbuffer(Output_Buffer_T *obp, int c);
extern int buf_putblock_outbuffer(Output_Buffer_T *obp, long length, char *byts);

extern void buf_flush(void);
extern void buf_flush_outbuffer(Output_Buffer_T *obp);

extern int buf_putstr(char const *str);
extern int buf_putstr_outbuffer(Output_Buffer_T *obp, char const *str);

extern long buf_printf_outbuffer(Output_Buffer_T *obp, long length, char const *fmt,
                                 ...);
extern long buf_printf(long length, char const *fmt, ...);

extern long buf_fill_char_outbuffer(Output_Buffer_T *obp, long length, int c);
extern long buf_fill_char(long length, int c);

#ifdef __cplusplus
}
#endif

#endif /* ifndef BUFFERIO_H */
