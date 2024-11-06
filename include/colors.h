#ifndef LASER_COLORS_H
#define LASER_COLORS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LASER_DEFAULT_RESET_COLOR "\x1b[0m"
#define LASER_DEFAULT_DIR_COLOR "\x1b[34m"
#define LASER_DEFAULT_SYMLINK_COLOR "\x1b[36m"
#define LASER_DEFAULT_FILE_COLOR "\x1b[38m"
#define LASER_DEFAULT_HIDDEN_COLOR "\x1b[90m"
#define LASER_DEFAULT_EXEC_COLOR "\x1b[32;4m"
#define LASER_DEFAULT_ARCHIVE_COLOR "\x1b[31m"
#define LASER_DEFAULT_MEDIA_COLOR "\x1b[33m"
#define LASER_DEFAULT_DOCUMENT_COLOR "\x1b[35;3m"

typedef struct laser_colors
{
    const char *reset;
    const char *dir;
    const char *symlink;
    const char *file;
    const char *hidden;
    const char *exec;
    const char *archive;
    const char *media;
    const char *documents;
} laser_colors;

typedef enum
{
    LASER_COLORKEY_RESET,
    LASER_COLORKEY_DIR,
    LASER_COLORKEY_SYMLINK,
    LASER_COLORKEY_FILE_KEY,
    LASER_COLORKEY_HIDDEN,
    LASER_COLORKEY_EXEC,
    LASER_COLORKEY_ARCHIVE,
    LASER_COLORKEY_MEDIA,
    LASER_COLORKEY_DOCUMENTS,
    LASER_COLORKEY_UNKNOWN
} laser_colorkey;

void laser_colors_init(void);
void laser_colors_parseToken(const char *token);
void laser_colors_destroy(void);

extern laser_colors *LASER_COLORS;

#endif
