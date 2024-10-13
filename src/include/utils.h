#ifndef LASER_UTILS_H
#define LASER_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct laser_opts
{
    int show_tree;
    int show_all;
    char *dir;
} laser_opts;

laser_opts laser_utils_parsecmd(int argc, char **argv);

#endif
