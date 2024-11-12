#ifndef LASER_FILETYPES_CHECKTYPES_H
#define LASER_FILETYPES_CHECKTYPES_H

#include <stddef.h>

struct laser_format_token
{
    unsigned char *token;
    size_t token_size;
};

int laser_checktype(const char *filename,
                    const struct laser_format_token formats[]);

extern const struct laser_format_token laser_archiveformats[];
extern const struct laser_format_token laser_mediaformats[];
extern const struct laser_format_token laser_documentformats[];

#endif
