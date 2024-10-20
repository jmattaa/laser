#ifndef LASER_UTILS_H
#define LASER_UTILS_H

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

typedef struct laser_opts
{
    int show_all;
    int show_files;
    int show_directories;
    int show_symlinks;
    int show_tree;
    char *dir;
} laser_opts;

laser_opts laser_utils_parsecmd(int argc, char **argv);
void laser_utils_format_date(time_t time, char *buffer, size_t buffer_size);
char **laser_utils_grow_strarray(char **array, size_t *alloc_size, size_t count);

#endif
