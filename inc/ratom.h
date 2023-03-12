/*====================================================================*/
/*
 * Copyright (C) 2018-2021 Richard Guo
 * Created on 2018/7/19
 */
/*====================================================================*/
/*====================================================================*/
/*
 * RAtom --- atomic bytes 
 */
/*====================================================================*/
#ifndef RATOM_H
#define RATOM_H 
#include "rdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/*========================================================================*/
/*
 * Gets length of atom.
 * 
 * Parameter: str -> the atomic string
 * 
 * Return: length of atom
 * 
 * Checked Runtime Error:
 *      str is NULL     -> RAssertFailed
 *      str is not atom -> RAssertFailed
 * 
 * Unchecked Runtime Error: None
 * Exception: None
 * 
 */
/*========================================================================*/
extern RLIB_EXPORT_FLAG int ratom_length(char const *str);

/*========================================================================*/
/*
 * Generates atomic sequence. 
 * Multiple identical byte sequences produce identical pointers.
 *
 * [0] ratom_new()
 * [1] ratom_string()
 *
 * Parameter: 
 *      str -> byte sequence
 *      len -> the length of str
 * 
 * Return: pointer, pointer to atom
 * 
 * Checked Runtime Error:
 *      [0][1] str is NULL -> RAssertFailed
 *      [0]    len < 0     -> RAssertFailed
 * 
 * Unchecked Runtime Error: None
 * Exception: None
 * 
 */
/*========================================================================*/
extern RLIB_EXPORT_FLAG char const *ratom_new(char const *str, int len);
extern RLIB_EXPORT_FLAG char const *ratom_string(char const *str);

/*========================================================================*/
/*
 * Generates atomic sequence for integer.
 *
 * Parameter: n -> the integer
 * 
 * Return: pinter, pointer to atom
 * 
 * Checked Runtime Error: None
 * Unchecked Runtime Error: 
 *      integer digits greater than 45 (greater than 8bytes)
 * 
 * Exception: None
 * 
 */
/*========================================================================*/
extern RLIB_EXPORT_FLAG char const *ratom_int(long n);

/*========================================================================*/
/*
 * Release atomic sequences.
 *
 * [0] ratom_free  -> release one
 * [1] ratom_reset -> release all
 * 
 * Parameter: [0] str -> the atom
 * 
 * Return: [0][1] void
 * Checked Runtime Error:
 *      [0]str is NULL     -> RAssertFailed
 *      [0]str is not atom -> RAssertFailed
 * 
 * Unchecked Runtime Error: None
 * Exception: None
 * 
 */
/*========================================================================*/
extern RLIB_EXPORT_FLAG void ratom_free(char const *str);
extern RLIB_EXPORT_FLAG void ratom_reset(void);

/*========================================================================*/
/*
 * Generates atomic sequences.
 *
 * [0] ratom_vload -> with va_arg
 * [1] ratom_aload -> with array
 *
 * Parameter: 
 *      [0] str -> the byte sequence
 *      [0] ... -> byte sequences
 *      [1] strs -> byte sequences array
 * 
 * Return: [0][1] void
 * 
 * Checked Runtime Error:
 *      [1] strs is NULL    -> RAssertFailed
 *      [1] strs[i] is NULL -> RAssertFailed
 * 
 * Unchecked Runtime Error: None
 * Exception: None
 * 
 */
/*========================================================================*/
extern RLIB_EXPORT_FLAG void ratom_vload(char const *str, ...);
extern RLIB_EXPORT_FLAG void ratom_aload(char const *strs[]);

/* client promise str don't release */
/*========================================================================*/
/*
 * Add byte sequences provided by the client to the atomic library.
 * Client promise str don't release.
 *
 * Parameter: 
 *      str -> the byte sequence
 *      len -> length of byte sequence
 * 
 * Return: pointer, pointer to atom
 * 
 * Checked Runtime Error: 
 *      str is NULL -> RAssertFailed
 *      len < 0     -> RAssertFailed
 * 
 * Unchecked Runtime Error:
 *      release str by the client
 * 
 * Exception: None
 * 
 */
/*========================================================================*/
extern RLIB_EXPORT_FLAG char const *ratom_add(char const *str, int len);

#ifdef __cplusplus
}
#endif

#endif /* ifndef RATOM_H */
