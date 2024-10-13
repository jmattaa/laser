#ifndef LASER_UTILS_H
#define LASER_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

typedef struct laser_opts
{
    int show_all;
    char *dir;
} laser_opts;

laser_opts laser_utils_parsecmd(int argc, char **argv);
void format_date(time_t time, char *buffer, size_t buffer_size);

#endif
