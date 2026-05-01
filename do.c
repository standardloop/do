#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

#include "./do.h"

#include <standardloop/logger.h>

extern void FreeDo(Do *do_value)
{
    if (do_value == NULL)
    {
        errno = EINVAL;
        return;
    }
    free(do_value);
}
