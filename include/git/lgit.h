#ifndef LASER_GIT_LGIT_H
#define LASER_GIT_LGIT_H

#include "../../include/utils.h"

#include <stdlib.h>
#include <stdint.h>

typedef struct
{
    char *name;
    unsigned tracked:1;

    mode_t mode; 
} lgit_entry;

typedef struct 
{
    char **lgit_entry;
} lgit_entries;

char **lgit_parseGitignore(const char *dir, int *count);

// lgit_entries *lgit_getGitEntries(laser_opts opts);
// void lgit_parseGit(char *dir);
// void lgit_entries_free(lgit_entries *lgit);

#endif
