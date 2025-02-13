#include "git/lgit.h"
#include "logger.h"
#include <git2/errors.h>
#include <git2/global.h>
#include <git2/status.h>

static void lgit_getDirsStatus(laser_opts opts, struct laser_dirent *entry,
                               const char *full_path)
{
    entry->git_status = ' ';

    git_status_options status_opts = GIT_STATUS_OPTIONS_INIT;
    status_opts.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
    status_opts.flags = GIT_STATUS_OPT_INCLUDE_UNTRACKED;

    git_status_list *status_list;
    if (git_status_list_new(&status_list, opts.git_repo, &status_opts) != 0)
    {
        laser_logger_error("failed to get status list for directory %s\n",
                           full_path);
        laser_logger_error("%s\n", git_error_last()->message);
        return;
    }

    size_t status_count = git_status_list_entrycount(status_list);
    for (size_t i = 0; i < status_count; i++)
    {
        const git_status_entry *status_entry =
            git_status_byindex(status_list, i);

        const char *path = status_entry->head_to_index
                               ? status_entry->head_to_index->new_file.path
                               : status_entry->index_to_workdir->new_file.path;

        if (strncmp(path, full_path, strlen(full_path)) == 0)
        {
            entry->git_status = status_entry->head_to_index ? 'M' : 'm';
            break;
        }
    }

    git_status_list_free(status_list);
}

#define INITIAL_CAPACITY 10
void lgit_getGitStatus(laser_opts opts, struct laser_dirent *entry,
                       const char *full_path)
{
    entry->git_status = ' ';

    // skip the leading "./" cuz libgit dosen't like it
    if (strncmp(full_path, "./", 2) == 0)
        full_path += 2;

    // so... git dosent track dirs, only files
    if (S_ISDIR(entry->s.st_mode))
    {
        lgit_getDirsStatus(opts, entry, full_path);
        return;
    }

    unsigned int status;
    if (git_status_file(&status, opts.git_repo, full_path) != 0)
    {
        laser_logger_error("failed to get status for %s\n", full_path);
        laser_logger_error("%s\n", git_error_last()->message);
        return;
    }

    switch (status)
    {
        case GIT_STATUS_WT_NEW:
            entry->git_status = 'a';
            break;
        case GIT_STATUS_WT_MODIFIED:
            entry->git_status = 'm';
            break;
        case GIT_STATUS_WT_RENAMED:
            entry->git_status = 'r';
            break;
        case GIT_STATUS_WT_TYPECHANGE:
            entry->git_status = 't';
            break;

            // staged changes
        case GIT_STATUS_INDEX_NEW:
            entry->git_status = 'A';
            break;
        case GIT_STATUS_INDEX_MODIFIED:
            entry->git_status = 'M';
            break;
        case GIT_STATUS_INDEX_RENAMED:
            entry->git_status = 'R';
            break;
        case GIT_STATUS_INDEX_TYPECHANGE:
            entry->git_status = 'T';
            break;
    }
}
