#ifndef LASER_LASER_H
#define LASER_LASER_H

#include "colors.h"
#include "utils.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_ENTRIES 1024

typedef struct
{
    char **dirs;

    char **files;

    char **symlinks;
    char **symlinks_targets;

    char **hidden;

    int dir_count;
    int file_count;
    int symlink_count;
    int hidden_count;
} laser_dir_entries;

laser_dir_entries laser_getdirs(laser_opts opts);

void laser_list(laser_dir_entries lentries);

#endif
