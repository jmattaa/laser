#include "logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void laser_logger_log(const char *format, ...)
{
    printf("lsr: ");
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
}

void laser_logger_error(const char *fmt, ...)
{
    fprintf(stderr, "\x1b[31mlsr\x1b[0m: ");
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}

void laser_logger_fatal(int err, const char *fmt, ...)
{
    fprintf(stderr, "lsr \x1b[31m[FATAL]\x1b[0m: ");
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
    exit(err);
}
