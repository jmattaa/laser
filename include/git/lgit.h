#ifndef LASER_GIT_LGIT_H
#define LASER_GIT_LGIT_H

#include "cli.h"
#include "laser.h"
#include <git2.h>
#include <stdint.h>

typedef struct lgit_ignorePatterns
{
    char **ignore;
    char **negated; // line starting with '!'
    int negated_count;
    int ignore_count;
} lgit_ignorePatterns;

lgit_ignorePatterns *lgit_parseGitignore(const char *dir, int depth);
void lgit_getGitStatus(laser_opts opts, struct laser_dirent *entry,
                       const char *full_path);
void lgit_destroyIgnorePatterns(lgit_ignorePatterns *patterns);

#endif
