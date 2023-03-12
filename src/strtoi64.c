/* #include <stdio.h> */
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>

#include "rassert.h"

#define INT64DIGIT_MAX 19
#define INT64HEX_MAX 16
#define INT64OCT_MAX 21
#define HEX_BIT 4
#define OCT_BIT 3

#define ISHEX(x)                                                                         \
    (isdigit((x)) || (x) == 'a' || (x) == 'b' || (x) == 'c' || (x) == 'd' ||             \
     (x) == 'e' || (x) == 'f' || (x) == 'A' || (x) == 'B' || (x) == 'C' || (x) == 'D' || \
     (x) == 'E' || (x) == 'F')

#define ISOCT(x) (isdigit((x)) && (x) != '8' && (x) != '9')

static int64_t convert_hex_to_num(int c)
{
    switch (c) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return (c - '0');
        case 'a':
        case 'A':
            return 0xaLL;
        case 'b':
        case 'B':
            return 0xbLL;
        case 'c':
        case 'C':
            return 0xcLL;
        case 'd':
        case 'D':
            return 0xdLL;
        case 'e':
        case 'E':
            return 0xeLL;
        case 'f':
        case 'F':
            return 0xfLL;
    }
    rassert(0);
    return 0;
}

int64_t strtoi64(char const *str, char **endptr, int base)
{
    char   *strp = (char *)str;
    int64_t res  = 0;
    int64_t mask = 0;
    int     di;
    int     sign = 1;

    rassert(str);
    rassert(base == 8 || base == 10 || base == 16);

    /* break space */
    while (isspace(*strp)) strp++;

    if (!(ISHEX(*strp) || *strp == '-' || *strp == '+')) {
        errno = EINVAL;
        return res;
    }

    if (*strp == '-') {
        sign = -1;
        strp++;
    } else if (*strp == '+')
        strp++;

    di = 0;
    switch (base) {
        case 8:
            /* break prefix */
            if (*strp == '0') strp++;

            while (ISOCT(*strp)) {
                if (++di > INT64OCT_MAX) {
                    errno = ERANGE;
                    res   = 0;
                    break;
                }
                mask = (int64_t)(*strp - '0');
                res <<= OCT_BIT;
                res |= mask;
                strp++;
            }
            break;
        case 16:
            /* break prefix */
            if (*strp == '0' && (*(strp + 1) == 'x' || *(strp + 1) == 'X')) strp += 2;

            while (ISHEX(*strp)) {
                if (++di > INT64HEX_MAX) {
                    errno = ERANGE;
                    res   = 0;
                    break;
                }
                res <<= HEX_BIT;
                mask = convert_hex_to_num(*strp);
                res |= mask;
                strp++;
            }
            break;
        case 10:
            while (*strp && isdigit(*strp)) {
                if (++di > INT64DIGIT_MAX) {
                    errno = ERANGE;
                    res   = 0;
                    break;
                }
                if (res > 0) res *= 10LL;
                if ((res += (int64_t)(*strp - '0')) < 0) {
                    errno = ERANGE;
                    res   = 0;
                    break;
                }
                strp++;
            }
            break;
    }

    if (di == 0) errno = EINVAL;

    if (endptr && errno == ERANGE)
        *endptr = (char *)str;
    else if (endptr)
        *endptr = strp;

    return (sign == -1) ? res * -1LL : res;
}
