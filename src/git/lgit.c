#include "include/lgit.h"
#include "include/utils.h"
#include <stdio.h>
#include <string.h>

#define GIT_INDEX_HEAD_LEN 48 // in bits

lgit_entries *lgit_getGitEntries(laser_opts opts)
{
    lgit_entries *entries = malloc(sizeof(lgit_entries));

    lgit_parseGit(opts.parentDir);

    return entries;
}

void lgit_parseGit(char *dir)
{
    // TODO: check if git dir exists!!
    char *index_file = malloc(strlen(dir) + strlen("/.git/index") + 1);

    snprintf(index_file, strlen(dir) + strlen("/.git/index") + 1,
             "%s/.git/index", dir);

    size_t idxfsize;
    char *idx_bincontent = lgit_utils_readbin(index_file, &idxfsize);
    char *idx_file_as_hex = lgit_utils_binToHex(idx_bincontent, idxfsize);

    // https://git-scm.com/docs/index-format
    // HEADER
    // first 4 bytes DIRC
    char HEADER_SIGNATURE[4] = {'D', 'I', 'R', 'C'};

    if (strcmp(lgit_utils_getChars(idx_bincontent, 0, 4 * 8),
               HEADER_SIGNATURE) != 0)
    {
        fprintf(stderr, "lsr: %s: file is not formatted right!!\n", index_file);
        return;
    }

    // next 4 bytes: version
    // 4 bytes = 8 hex nums
    uint32_t version =
        lgit_utils_hexToUint32(lgit_utils_getChars(idx_file_as_hex, 8, 8));

    // then 32 bit num for index entries
    // 32 bit 4 byte
    uint32_t entries_num =
        lgit_utils_hexToUint32(lgit_utils_getChars(idx_file_as_hex, 16, 8));

    printf("version: %u, entries_count: %u\n", version, entries_num);
}

void lgit_entries_free(lgit_entries *lgit)
{
    free(lgit);
}
