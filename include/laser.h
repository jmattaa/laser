#ifndef LASER_LASER_H
#define LASER_LASER_H

#include "cli.h"
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

    char git_status;
};

void laser_start(laser_opts opts);
void laser_process_single_file(laser_opts opts);

#endif
