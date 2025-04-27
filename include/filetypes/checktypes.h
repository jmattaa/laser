#ifndef LASER_FILETYPES_CHECKTYPES_H
#define LASER_FILETYPES_CHECKTYPES_H

#include <stddef.h>

// https://en.wikipedia.org/wiki/List_of_file_signatures
// dis be good sometimes

struct laser_filetype
{
    unsigned char *magic;
    long magic_size;

    const char **extensions;
    size_t extensions_count;
};

int laser_checktype(const char *filename,
                    const struct laser_filetype formats[]);

int laser_checktype_extension(const char *filename,
                              const struct laser_filetype formats[]);
int laser_checktype_magic(int fd, const struct laser_filetype formats[]);

extern const struct laser_filetype laser_archiveformats[];
extern const struct laser_filetype laser_mediaformats[];
extern const struct laser_filetype laser_documentformats[];

#endif
