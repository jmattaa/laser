#include "git/lgit.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

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

// NO NEED FOR THESE NOW
// --------------------------------------------------
// lgit_entries *lgit_getGitEntries(laser_opts opts)
// {
//     lgit_entries *entries = malloc(sizeof(lgit_entries));
//
//     lgit_parseGit(opts.parentDir);
//
//     return entries;
// }
//
// void lgit_parseGit(char *dir)
// {
//     // TODO: check if git dir exists!!
//     char *index_file = malloc(strlen(dir) + strlen("/.git/index") + 1);
//
//     snprintf(index_file, strlen(dir) + strlen("/.git/index") + 1,
//              "%s/.git/index", dir);
//
//     size_t idxfsize;
//     char *idx_bincontent = lgit_utils_readbin(index_file, &idxfsize);
//     char *idx_file_as_hex = lgit_utils_binToHex(idx_bincontent, idxfsize);
//
//     // https://git-scm.com/docs/index-format
//     // HEADER
//     // first 4 bytes DIRC
//     char HEADER_SIGNATURE[4] = {'D', 'I', 'R', 'C'};
//
//     if (strcmp(lgit_utils_getChars(idx_bincontent, 0, 4 * 8),
//                HEADER_SIGNATURE) != 0)
//     {
//         fprintf(stderr, "lsr: %s: file is not formatted right!!\n", index_file);
//         return;
//     }
//
//     // next 4 bytes: version
//     // 4 bytes = 8 hex nums
//     uint32_t version =
//         lgit_utils_hexToUint32(lgit_utils_getChars(idx_file_as_hex, 8, 8));
//
//     // then 32 bit num for index entries
//     // 32 bit 4 byte
//     uint32_t entries_num =
//         lgit_utils_hexToUint32(lgit_utils_getChars(idx_file_as_hex, 16, 8));
//
//     printf("version: %u, entries_count: %u\n", version, entries_num);
// }
//
// void lgit_entries_free(lgit_entries *lgit)
// {
//     free(lgit);
// }
//
