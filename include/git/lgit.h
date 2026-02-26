#ifndef LASER_GIT_LGIT_H
#define LASER_GIT_LGIT_H

#include "laser.h"
#include <git2.h>
#include <stdint.h>

void lgit_getGitStatus(git_repository *repo, struct laser_dirent *entry,
                       const char *full_path);

#endif
