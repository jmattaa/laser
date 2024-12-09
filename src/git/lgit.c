#include "git/lgit.h"
#include <git2/errors.h>
#include <git2/global.h>
#include <git2/status.h>

#define INITIAL_CAPACITY 10

lgit_ignorePatterns *lgit_parseGitignore(const char *dir, int depth)
{
    size_t filename_len = strlen(dir) + strlen("/.gitignore") + 1;
    char *filename = malloc(filename_len);

    snprintf(filename, filename_len, "%s/.gitignore", dir);
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        // if we're not in root dir for repo then we dont need to have a
        // .gitnignore so don't print an error
        if (!(depth > 0))
            perror("lsr could not open .gitignore");

        free(filename);
        return NULL;
    }

    lgit_ignorePatterns *ignored_patterns = malloc(sizeof(lgit_ignorePatterns));
    if (!ignored_patterns)
    {
        perror("lsr could not allocate memory for ignored patterns");
        fclose(file);
        free(filename);
        return NULL;
    }

    ignored_patterns->ignore = malloc(INITIAL_CAPACITY * sizeof(char *));
    ignored_patterns->negated = malloc(INITIAL_CAPACITY * sizeof(char *));
    if (!ignored_patterns->ignore || !ignored_patterns->negated)
    {
        perror("lsr could not allocate memory for ignored patterns");
        goto fail;
    }

    ignored_patterns->ignore_count = 0;
    ignored_patterns->negated_count = 0;

    size_t capacity = INITIAL_CAPACITY, neg_capacity = INITIAL_CAPACITY;
    char line[LASER_PATH_MAX];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        char *newline = strchr(line, '\n');
        if (newline)
            *newline = '\0';

        // TODO: HANDLE '*'
        switch (line[0])
        {
            case '#': // skip comments and empty lines
            case '\0':
                break;
            case '!':
                if (ignored_patterns->negated_count >= neg_capacity)
                {
                    neg_capacity *= 2;
                    ignored_patterns->negated =
                        realloc(ignored_patterns->negated,
                                neg_capacity * sizeof(char *));
                    if (!ignored_patterns->negated)
                    {
                        perror("lsr could not allocate memory for ignored "
                               "patterns");
                        goto fail;
                    }
                }

                ignored_patterns->negated_count++;
                ignored_patterns->negated[ignored_patterns->negated_count - 1] =
                    strdup(line + 1); // Skip the '!' character
                break;
            default:
                if (ignored_patterns->ignore_count >= capacity)
                {
                    capacity *= 2;
                    ignored_patterns->ignore = realloc(
                        ignored_patterns->ignore, capacity * sizeof(char *));
                    if (!ignored_patterns->ignore)
                    {
                        perror("lsr could not allocate memory for ignored "
                               "patterns");
                        goto fail;
                    }
                }

                ignored_patterns->ignore_count++;
                ignored_patterns->ignore[ignored_patterns->ignore_count - 1] =
                    strdup(line);
                break;
        }
    }

    free(filename);
    fclose(file);

    return ignored_patterns;

fail:
    for (size_t i = 0; i < ignored_patterns->ignore_count; i++)
        free(ignored_patterns->ignore[i]);
    for (size_t i = 0; i < ignored_patterns->negated_count; i++)
        free(ignored_patterns->negated[i]);

    fclose(file);
    free(filename);
    free(ignored_patterns->ignore);
    free(ignored_patterns->negated);
    free(ignored_patterns);
    return NULL;
}

void lgit_getGitStatus(laser_opts opts, struct laser_dirent *entry,
                       const char *full_path)
{
    if (!opts.show_git)
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

    char status_char = ' ';
    if (status & GIT_STATUS_WT_NEW)
        status_char = 'A';
    else if (status & GIT_STATUS_WT_MODIFIED)
        status_char = 'M';
    else if (status & GIT_STATUS_WT_DELETED)
        status_char = 'D';
    else if (status & GIT_STATUS_WT_RENAMED)
        status_char = 'R';
    else if (status & GIT_STATUS_WT_TYPECHANGE)
        status_char = 'T';

    entry->git_status = status_char;
}

void lgit_destroyIgnorePatterns(lgit_ignorePatterns *ignored_patterns)
{
    for (int i = 0; i < ignored_patterns->ignore_count; i++)
        free(ignored_patterns->ignore[i]);
    for (int i = 0; i < ignored_patterns->negated_count; i++)
        free(ignored_patterns->negated[i]);
    free(ignored_patterns->ignore);
    free(ignored_patterns->negated);
    free(ignored_patterns);
}
