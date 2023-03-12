#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include "rtools.h"
#include "rtest.h"


int main(int argc, char *argv[])
{
    int32_t a1, a2, a3, a4, a5, a6;
    a1 = 123;
    a2 = 12345;
    a3 = 0;
    a4 = -1;
    a5 = INT32_MIN;
    a6 = INT32_MAX;
    int res;


    /* 123 returned 3 */
    res = rtools_get_number_of_digit(a1, 10);
    fprintf(stderr, "a1: %d get nums: %d is [%s]\n", a1, res,
            RTEST_STATUS_STR(res == 3));

    /* 12345 returned 5 */
    res = rtools_get_number_of_digit(a2, 10);
    fprintf(stderr, "a2: %d get nums: %d is [%s]\n", a2, res,
            RTEST_STATUS_STR(res == 5));

    /* 0 returned 0 */
    res = rtools_get_number_of_digit(a3, 10);
    fprintf(stderr, "a3: %d get nums: %d is [%s]\n", a3, res,
            RTEST_STATUS_STR(res == 0));

    /* -1 returned 1 */
    res = rtools_get_number_of_digit(a4, 10);
    fprintf(stderr, "a4: %d get nums: %d is [%s]\n", a4, res,
            RTEST_STATUS_STR(res == 1));

    /* 32 bits INT_MIN is returend 10 */
    res = rtools_get_number_of_digit(a5, 10);
    fprintf(stderr, "a5: %d get nums: %d is [%s]\n", a5, res,
            RTEST_STATUS_STR(res == 10));


    /* 32 bits INT_MAX is return 10 */
    res = rtools_get_number_of_digit(a6, 10);
    fprintf(stderr, "a6: %d get nums: %d is [%s]\n", a6, res,
            RTEST_STATUS_STR(res == 10));

    /* 你 utf-8 bytes: e4 bd a0 */
    /* char utf8_str[] = "你"; */
    /* char *bits = rtools_bytes_to_bits_string(utf8_str, sizeof (utf8_str)); */
    /* rtools_print_bits_string(bits, sizeof (utf8_str)); */

    /* bits = rtools_integer_to_bits_string(utf8_str, sizeof (utf8_str)); */
    /* rtools_print_bits_string(bits, sizeof (utf8_str)); */

    return 0;
}
