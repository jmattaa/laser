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

#define LASER_COLORS_ITER(_X)                                             \
    _X(RESET, "\x1b[0m")                                                       \
    _X(DIR, "\x1b[34m")                                                        \
    _X(SYMLINK, "\x1b[36m")                                                    \
    _X(FILE, "\x1b[0m")                                                        \
    _X(HIDDEN, "\x1b[90m")                                                     \
    _X(EXEC, "\x1b[32;4m")                                                     \
    _X(ARCHIVE, "\x1b[31m")                                                    \
    _X(MEDIA, "\x1b[33m")                                                      \
    _X(DOCUMENT, "\x1b[35;3m")

#define _X(name, value) {#name, value},

static const laser_color LASER_COLORS_DEFAULTS[COLOR_COUNT] = {
    LASER_COLORS_ITER(_X)
};

#undef _X
#define _X(name, value) LASER_COLOR_##name,

typedef enum laser_color_type
{
    LASER_COLORS_ITER(_X)
} laser_color_type;

extern laser_color *LASER_COLORS;

void laser_colors_init(void);
void laser_colors_parseToken(const char *token);
void laser_colors_free(void);

#undef _X
#undef LASER_COLORS_ITER
#endif
