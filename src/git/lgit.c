#include "git/lgit.h"
#include <git2/errors.h>
#include <git2/global.h>
#include <git2/status.h>

#define MAX_LINE_LENGTH 1024
#define INITIAL_CAPACITY 10

char **lgit_parseGitignore(const char *dir, int *count)
{
    size_t filename_len = strlen(dir) + strlen("/.gitignore") + 1;
    char *filename = malloc(filename_len);

    snprintf(filename, filename_len, "%s/.gitignore", dir);
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Could not open .gitignore");
        free(filename);
        return NULL;
    }

    char **ignored_patterns = malloc(INITIAL_CAPACITY * sizeof(char *));
    if (!ignored_patterns)
    {
        perror("Could not allocate memory for ignored patterns");
        fclose(file);
        free(filename);
        return NULL;
    }

    size_t capacity = INITIAL_CAPACITY;
    *count = 0;

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file))
    {
        char *trimmed = strtok(line, "\n");
        if (trimmed && trimmed[0] != '#' && trimmed[0] != '\0')
        {
            if (*count >= capacity)
            {
                capacity *= 2;
                ignored_patterns =
                    realloc(ignored_patterns, capacity * sizeof(char *));
                if (!ignored_patterns)
                {
                    perror("Could not reallocate memory for ignored patterns");
                    fclose(file);
                    free(filename);
                    return NULL;
                }
            }
            ignored_patterns[*count] = strdup(trimmed);
            (*count)++;
        }
    }

    free(filename);
    fclose(file);
    return ignored_patterns;
}

void lgit_getGitStatus(laser_opts opts, struct laser_dirent *entry,
                       const char *full_path)
{
    if (!opts.show_git)
        return;

    unsigned int status;
    if (git_status_file(&status, opts.git_repo, full_path) != 0)
    {
        fprintf(stderr, "lsr: failed to get status for %s\n", full_path);
        fprintf(stderr, "lsr: %s\n", git_error_last()->message);
        exit(1);
    }

    char status_char = ' ';
    if (status & GIT_STATUS_INDEX_NEW)
        status_char = 'A';
    else if (status & GIT_STATUS_INDEX_MODIFIED)
        status_char = 'M';
    else if (status & GIT_STATUS_INDEX_DELETED)
        status_char = 'D';
    else if (status & GIT_STATUS_INDEX_RENAMED)
        status_char = 'R';
    else if (status & GIT_STATUS_INDEX_TYPECHANGE)
        status_char = 'T';

    entry->git_status = status_char;
}
