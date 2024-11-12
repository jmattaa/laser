#ifndef LASER_COLORS_H
#define LASER_COLORS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COLOR_COUNT 9

typedef struct
{
    const char *key;
    const char *value;
} laser_color;

static const laser_color LASER_COLORS_DEFAULTS[COLOR_COUNT] = {
    {"RESET", "\x1b[0m"},      {"DIR", "\x1b[34m"},
    {"SYMLINK", "\x1b[36m"},   {"FILE", "\x1b[0m"},
    {"HIDDEN", "\x1b[90m"},    {"EXEC", "\x1b[32;4m"},
    {"ARCHIVE", "\x1b[31m"},   {"MEDIA", "\x1b[33m"},
    {"DOCUMENT", "\x1b[35;3m"}};

enum laser_color_type
{
    LASER_COLOR_RESET,
    LASER_COLOR_DIR,
    LASER_COLOR_SYMLINK,
    LASER_COLOR_FILE,
    LASER_COLOR_HIDDEN,
    LASER_COLOR_EXEC,
    LASER_COLOR_ARCHIVE,
    LASER_COLOR_MEDIA,
    LASER_COLOR_DOCUMENT
};

extern laser_color *LASER_COLORS;

void laser_colors_init(void);
void laser_colors_parsesignature(const char *signature);
void laser_colors_destroy(void);

#endif
