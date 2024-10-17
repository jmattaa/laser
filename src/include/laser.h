#ifndef LASER_LASER_H
#define LASER_LASER_H

#include "colors.h"
#include "utils.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_ENTRIES 1024

typedef struct laser_dir_entries laser_dir_entries;

typedef struct 
{
    char *name;
    laser_dir_entries *sub_entires;
} laser_dir;

struct  laser_dir_entries
{
    laser_dir **dirs;

    char **files;

    char **symlinks;
    char **symlinks_targets;

    char **hidden;

    int dir_count;
    int file_count;
    int symlink_count;
    int hidden_count;
};

laser_dir_entries laser_getdirs(laser_opts opts);

void laser_list(laser_dir_entries lentries);

laser_dir *laser_init_dir(char *name);
laser_dir **laser_grow_dirarray(laser_dir **dirs, size_t *alloc_size, size_t count);
void laser_free_dir(laser_dir *dir);

#endif
