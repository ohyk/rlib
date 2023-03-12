#include "bufferio.h"

#include <assert.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "rmem.h"

#define RMIN(x, y) ((x) < (y) ? (x) : (y))

Input_Buffer_T  Global_Input_Buffer;
Output_Buffer_T Global_Output_Buffer;

static Input_Buffer_T *get_global_inputbuffer(void)
{
    if (Global_Input_Buffer.in == NULL) Global_Input_Buffer.in = stdin;
    return &Global_Input_Buffer;
}

static Output_Buffer_T *get_global_outputbuffer(void)
{
    if (Global_Output_Buffer.out == NULL) Global_Output_Buffer.out = stdout;
    return &Global_Output_Buffer;
}

static char *ensure_outbuf(Output_Buffer_T *const outbuf, size_t needed)
{
    char  *newbuf;
    size_t newlen;

    if (!outbuf) return NULL;

    if (needed > INT_MAX) return NULL;

    if (outbuf->offset > outbuf->length) return NULL;

    if (needed < outbuf->length - outbuf->offset) {
        outbuf->offset += needed;
        return outbuf->buf + (outbuf->offset - needed);
    }

    if (needed < (INT_MAX / 2))
        newlen = needed + 1024 + outbuf->length;
    else
        newlen = needed + outbuf->length;

    if (!outbuf->buf) {
        newbuf      = ALLOC(newlen);
        outbuf->buf = newbuf;
    } else {
        RESIZE(outbuf->buf, newlen);
    }

    outbuf->length = newlen;
    outbuf->offset += needed;
    return outbuf->buf + (outbuf->offset - needed);
}

static void input_buffer_read(Input_Buffer_T *buf)
{
    int n;
    if ((n = fread(buf->buf, 1, INPUT_BUF_SIZE, buf->in)) > 0) {
        /* buf->bufptr = buf->buf; */
        buf->offset = 0;
        buf->count  = n;
    } else if (feof(buf->in)) {
        buf->count = BUFFER_EOF;
    } else {
        assert(!"fread error");
    }
}

/*!	TODO(rich): Todo description here
 *	\todo Todo description here
 */
/* static int ensure_inbuf(Input_Buffer_T *ibp, long needed, FILE *fp) */
/* { */
/* if (feof(fp)) */
/* return BUFFER_EOF; */
/* if (ibp->count < needed) */
/* input_buffer_read(ibp, fp); */
/* return needed; */
/* } */

int buf_getchar_inbuffer(Input_Buffer_T *ibp)
{
    assert(ibp);
    if (ibp->count == BUFFER_EOF && !feof(ibp->in)) ibp->count = 0;
    while (ibp->count != BUFFER_EOF) {
        if (ibp->count > 0) {
            ibp->count--;
            return ibp->buf[ibp->offset++];
        } else {
            input_buffer_read(ibp);
        }
    }
    return EOF;
}

void buf_ungetchar_inbuffer(Input_Buffer_T *ibp, int c)
{
    assert(ibp);

    if (ibp->count < INPUT_BUF_SIZE && ibp->offset > 0) {
        ibp->buf[--ibp->offset] = c;
    } else if (ibp->count < INPUT_BUF_SIZE && ibp->offset == 0) {
        ibp->buf[ibp->offset] = c;
    } else {
        assert(0);
    }
    if (ibp->count == BUFFER_EOF) ibp->count = 0;
    ibp->count++;
}

/* long buf_getline_inbuffer_fp(Input_Buffer_T *ibp, long length, char *line, FILE *fp) */
/* { */
/* assert(ibp); */
/* assert(length > 0); */
/* assert(line); */
/* } */

long buf_getblock_inbuffer(Input_Buffer_T *ibp, long length, char *block)
{
    assert(ibp);
    assert(length > 0);
    assert(block);
    int  n;
    long count = 0;

    /* buffer empty EOF false */
    if (ibp->count == 0) input_buffer_read(ibp);

    /* clean EOF and empty */
    if (ibp->count == BUFFER_EOF && !feof(ibp->in)) input_buffer_read(ibp);

    n = RMIN(length, ibp->count);

    if (n == length) {
        memcpy(block, ibp->buf + ibp->offset, n);
        ibp->count -= n;
        ibp->offset += n;
        length -= n;
        count += n;
    }

    while (length > 0 && ibp->count != BUFFER_EOF) {
        n = RMIN(length, ibp->count);
        memcpy(block, ibp->buf + ibp->offset, n);
        block += n;
        length -= n;
        count += n;
        ibp->offset += n;
        ibp->count -= n;
        if (n == ibp->count) input_buffer_read(ibp);
    }
    return count;
}

int buf_getchar(void) { return buf_getchar_inbuffer(get_global_inputbuffer()); }

void buf_ungetchar(int c) { buf_ungetchar_inbuffer(get_global_inputbuffer(), c); }

long buf_getblock(long length, char *block)
{
    return buf_getblock_inbuffer(get_global_inputbuffer(), length, block);
}

int buf_putchar_outbuffer(Output_Buffer_T *obp, int c)
{
    assert(obp);
    char *buf = ensure_outbuf(obp, 1);
    if (buf) *buf = c;
    return buf != NULL;
}

int buf_putstr_outbuffer(Output_Buffer_T *obp, char const *str)
{
    char *buf;
    int   n;
    assert(obp);
    assert(str);
    n   = strlen(str);
    buf = ensure_outbuf(obp, n);
    if (buf) memcpy(buf, str, n);
    return buf != NULL;
}

int buf_putblock_outbuffer(Output_Buffer_T *obp, long length, char *byts)
{
    assert(obp);
    assert(length > 0);
    assert(byts);
    char *buf = ensure_outbuf(obp, length);
    if (buf) memcpy(buf, byts, length);
    return buf != NULL;
}

int buf_putchar(int c) { return buf_putchar_outbuffer(get_global_outputbuffer(), c); }

int buf_putblock(long length, char *byts)
{
    return buf_putblock_outbuffer(get_global_outputbuffer(), length, byts);
}

int buf_putstr(char const *str)
{
    return buf_putstr_outbuffer(get_global_outputbuffer(), str);
}

void buf_flush_outbuffer(Output_Buffer_T *obp)
{
    assert(obp);
    if (fwrite(obp->buf, 1, obp->offset, obp->out) != obp->offset) {
        fprintf(stderr, "%s fwrite fault\n", __func__);
    }
    obp->offset = 0;
}

void buf_flush(void) { buf_flush_outbuffer(get_global_outputbuffer()); }

int buf_unputchar_outbuffer(Output_Buffer_T *obp)
{
    assert(obp);
    if (!obp->buf) return BUFFER_EOF;
    if (obp->offset > obp->length) return BUFFER_EOF;
    if (obp->offset > 0) {
        return obp->buf[--obp->offset];
    } else {
        return BUFFER_EOF;
    }
}

int buf_unputblock_outbuffer(Output_Buffer_T *obp, long length, char *byts)
{
    assert(obp);
    assert(length > 0);
    if (!obp->buf) return BUFFER_EOF;
    if (obp->offset > obp->length) return BUFFER_EOF;
    if (length > obp->offset) return BUFFER_EOF;
    obp->offset -= length;
    if (byts) memcpy(byts, obp->buf + obp->offset, length);
    return length;
}

int buf_unputchar(void) { return buf_unputchar_outbuffer(get_global_outputbuffer()); }

int buf_unputblock(long length, char *byts)
{
    return buf_unputblock_outbuffer(get_global_outputbuffer(), length, byts);
}

void buf_cleanup_outbuffer(Output_Buffer_T *obp)
{
    assert(obp);
    obp->offset = 0;
}

static long vsnprintf_outbuffer(Output_Buffer_T *obp, long length, char const *fmt,
                                va_list va)
{
    char *buf = ensure_outbuf(obp, length);
    long  n;
    if (!buf) return BUFFER_EOF;
    if (obp->offset > obp->length) return BUFFER_EOF;
    n = vsnprintf(buf, length, fmt, va);
    /* clean \0 */
    if (n > length) {
        n = length;
        obp->offset--;
    } else if (n < length) {
        obp->offset -= (length - n);
    } else {
        obp->offset--;
    }
    return n;
}

long buf_printf_outbuffer(Output_Buffer_T *obp, long length, char const *fmt, ...)
{
    va_list va;
    long    n;
    assert(obp);
    assert(length > 0);
    va_start(va, fmt);
    n = vsnprintf_outbuffer(obp, length, fmt, va);
    va_end(va);
    return n;
}

long buf_printf(long length, char const *fmt, ...)
{
    va_list va;
    long    n;
    assert(length > 0);
    va_start(va, fmt);
    n = vsnprintf_outbuffer(get_global_outputbuffer(), length, fmt, va);
    va_end(va);
    return n;
}

long buf_fill_char_outbuffer(Output_Buffer_T *obp, long length, int c)
{
    assert(obp);
    assert(length > 0);
    char *buf = ensure_outbuf(obp, length);
    if (!buf) return BUFFER_EOF;
    if (obp->length < obp->offset) return BUFFER_EOF;
    memset(buf, c, length);
    return length;
}

long buf_fill_char(long length, int c)
{
    return buf_fill_char_outbuffer(get_global_outputbuffer(), length, c);
}
