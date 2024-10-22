#include "include/lgit.h"
#include <string.h>

lgit_entries *lgit_getGitEntries(laser_opts opts)
{
    lgit_entries *entries = malloc(sizeof(lgit_entries *));

    lgit_parseGit(opts.parentDir);

    return entries;
}

void lgit_parseGit(char *dir)
{
    // TODO: check if git dir exists!!
    char *index_file = malloc(strlen(dir) + strlen("/.git/index") + 1);

    snprintf(index_file, strlen(dir) + strlen("/.git/index") + 1,
             "%s/.git/index", dir);

    printf("index file at: %s\n", index_file);
}

void lgit_entries_free(lgit_entries *lgit)
{
    free(lgit);
}
