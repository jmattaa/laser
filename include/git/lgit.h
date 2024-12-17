#ifndef LASER_GIT_LGIT_H
#define LASER_GIT_LGIT_H

#include "cli.h"
#include "laser.h"
#include <git2.h>
#include <stdint.h>

void lgit_getGitStatus(laser_opts opts, struct laser_dirent *entry,
                       const char *full_path);
void lgit_getDirStatus(laser_opts opts, struct laser_dirent *entry,
                       const char *full_path);

#endif
