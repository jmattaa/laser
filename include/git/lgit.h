#ifndef LASER_GIT_LGIT_H
#define LASER_GIT_LGIT_H

#include "cli.h"
#include "laser.h"
#include <git2.h>
#include <stdint.h>

char **lgit_parseGitignore(const char *dir, int *count);
void lgit_getGitStatus(laser_opts opts, struct laser_dirent *entry,
                       const char *full_path);

#endif
