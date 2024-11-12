#ifndef LASER_LASER_H
#define LASER_LASER_H

#include "colors.h"
#include "sort.h"
#include "utils.h"
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#define LASER_PATH_MAX 1024

// dirent go brrrr 🔥
struct laser_dirent
{
    struct dirent *d;
    struct stat s;
};

void laser_list_directory(laser_opts opts, int depth, int max_depth);
void laser_print_entry(struct laser_dirent *entry, const char *color, char *indent,
                       int depth, laser_opts opts,
                       int is_last);

#endif
