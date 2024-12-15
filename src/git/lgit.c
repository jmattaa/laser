#include "git/lgit.h"
#include <git2/errors.h>
#include <git2/global.h>
#include <git2/status.h>

#define INITIAL_CAPACITY 10

void lgit_getGitStatus(laser_opts opts, struct laser_dirent *entry,
                       const char *full_path)
{
    entry->git_status = ' ';
    // so... git dosent track dirs, only files
    if (S_ISDIR(entry->s.st_mode))
        return;

    // skip the leading "./" cuz libgit dosen't like it
    if (strncmp(full_path, "./", 2) == 0)
        full_path += 2;

    unsigned int status;
    if (git_status_file(&status, opts.git_repo, full_path) != 0)
    {
        fprintf(stderr, "lsr: failed to get status for %s\n", full_path);
        fprintf(stderr, "lsr: %s\n", git_error_last()->message);
        return;
    }

    if (status & GIT_STATUS_WT_NEW)
        entry->git_status = 'A';
    else if (status & GIT_STATUS_WT_MODIFIED)
        entry->git_status = 'M';
    else if (status & GIT_STATUS_WT_RENAMED)
        entry->git_status = 'R';
    else if (status & GIT_STATUS_WT_TYPECHANGE)
        entry->git_status = 'T';
}
