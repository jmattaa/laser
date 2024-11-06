#ifndef LASER_FILETYPES_CHECKTYPES_H
#define LASER_FILETYPES_CHECKTYPES_H

#include <stddef.h>

struct laser_magicnumber
{
    unsigned char *magic;
    size_t magic_size;
};

int laser_checktype(const char *filename,
                    const struct laser_magicnumber formats[]);

extern const struct laser_magicnumber laser_archiveformats[];
extern const struct laser_magicnumber laser_mediaformats[];
extern const struct laser_magicnumber laser_documentformats[];

#endif
