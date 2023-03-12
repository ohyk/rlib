#include <stdio.h>
#include <errno.h>
#include "rlog.h"

int main(int argc, char *argv[])
{
    // stderr

    /* rlog_reset("TEST_LOG_FILE.txt"); */

    errno = 1;
    rlog_message("hahhaah failed");
    errno = 2;
    rlog_message("skks failed");

    errno = ERANGE;
    rlog_error_message("err msg failed");

    rlog_usage_error("This time is ok\nopen failed\nSS\n");

    errno = EINVAL;
    rlog_exit("bad bad bad");

    return 0;
}
