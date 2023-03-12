#ifndef RTOOLS_H
#define RTOOLS_H
#include "rdef.h"
#include "rexcept.h"

#ifndef RTOOLS_ALLOC
#define RTOOLS_ALLOC ALLOC
#endif /* ifndef RTOOLS_ALLOC */

#ifndef RTOOLS_FREE
#define RTOOLS_FREE FREE
#endif /* ifndef RTOOLS_FREE */

/*====================================================================*/
/*
 * Converting integer range overflow raise this exception
 */
/*====================================================================*/
extern RLIB_EXPORT_FLAG RExcept_T const RToolsRangeOverflow;

/*====================================================================*/
/*
 * Convert string to long integer (as identified by calling strtol())
 *
 * Discards any whitespace characters until the first non-whitespace 
 * character is found, then takes as many character as possible to 
 * form a valid base-n integer number representation and converts them
 * to an integer value.
 *
 * base: 0, 8, 16
 *
 * Return:
 *      - long int
 * 
 * Checked Runtime Error:
 *      - base invalid                      --> RAssertFailed
 *      - str is NULL                       --> RAssertFailed
 *      - greater than long integer range   --> RToolsRangeOverflow
 *      - less than long integer range      --> RToolsRangeOverflow
 *      - str is invalid                    --> RAssertFailed
 *      - str first non-whitespace char not 
 *        digit or '+/-'                    --> RAssertFailed
 * 
 * Unchecked Runtime Error:
 *      - str not include terminator '\0'
 *      - endp is invalid pointer
 * 
 * Exception: None
 * 
 */
/*====================================================================*/
extern RLIB_EXPORT_FLAG long rtools_string_to_long_endp(char const *str, char **endp,
                                                        int base);
extern RLIB_EXPORT_FLAG long rtools_string_to_long(char const *str, int base);

/*====================================================================*/
/*
 * Count fd lines offset
 *
 * Return:
 *      lines offset array, terminator is -1
 * 
 * Checked Runtime Error:
 *      - fd < 0                --> RAssertFailed
 *      - fd lseek() failed     --> RAssertFailed
 * 
 * Unchecked Runtime Error: None
 * 
 * Exception: None
 * 
 */
/*====================================================================*/
extern RLIB_EXPORT_FLAG int *rtools_get_line_numbers_fd(int fd);

extern RLIB_EXPORT_FLAG int rtools_get_number_of_digit(int n, int base);

extern RLIB_EXPORT_FLAG char **rtools_split_string_with_delimiter(char const *str,
                                                                  int len, int dmt);

extern RLIB_EXPORT_FLAG char *rtools_squeeze_repeated_string_one(char *str, int len,
                                                                 int (*isc)(int));

extern RLIB_EXPORT_FLAG void *rword_binary_search(void *array, size_t elem_size,
                                                  size_t elem_count, void const *key,
                                                  int (*cmp)(void const *, void const *));
extern RLIB_EXPORT_FLAG void  rtools_shell_sort(void *array, size_t elem_size,
                                                size_t elem_count,
                                                int (*cmp)(void const *, void const *));
extern RLIB_EXPORT_FLAG void  rtools_disrupt_array(void *array, size_t elem_size,
                                                   size_t elem_count);
extern RLIB_EXPORT_FLAG char *rtools_string_center(char const *str, int len, int padc);

/*========================================================================*/
/*
 * Simple regular expression match, like grep.
 *
 * Metacharacters:
 *      c - char 
 *      . - match any single character 
 *      ^ - match string start
 *      $ - match string end
 *      * - match zero or more occurrences of the previous character
 *
 * Return:
 *      1 - match 
 *      0 - nomatch
 * 
 * Checked Runtime Error:
 *      regexp is NULL      --> RAssertFailed
 *      text is NULL        --> RAssertFailed
 * 
 * Unchecked Runtime Error:
 *      regexp or text do not contain '\0'
 * 
 * Exception: None
 * 
 */
/*========================================================================*/
extern RLIB_EXPORT_FLAG int rtools_match(char const *regexp, char const *text);

extern RLIB_EXPORT_FLAG int *rtools_kmp_make_jump(char const *dstr, int len);
extern RLIB_EXPORT_FLAG int  rtools_kmp_find_string(char const *str, char const *dstr,
                                                    int const *jump);

#endif /* ifndef RTOOLS_H */
