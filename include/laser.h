#ifndef LASER_LASER_H
#define LASER_LASER_H

#include "colors.h"
#include "sort.h"
#include "utils.h"
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>

#define LASER_PATH_MAX 1024

void laser_list_directory(laser_opts opts, int depth);
void laser_print_entry(const char *name, const char *color, char *indent,
                       int depth, struct stat file_stat, laser_opts opts,
                       int is_last);

#endif
