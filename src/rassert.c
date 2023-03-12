#include "rassert.h"

RExcept_T const RAssertFailed = { "Assertion failed" };

void (rassert)(int e)
{
    rassert(e);
}
