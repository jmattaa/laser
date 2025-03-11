#include "logger.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define _X(level, color, fd, sign, err)                                        \
    void laser_logger_##level sign                                             \
    {                                                                          \
        fprintf(fd, color "lsr [" #level "]\x1b[0m: ");                         \
        va_list args;                                                          \
        va_start(args, fmt);                                                   \
        vfprintf(fd, fmt, args);                                               \
        va_end(args);                                                          \
        err                                                                    \
    }
LOG_LEVEL_ITER(_X)
#undef _X
