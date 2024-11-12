#ifndef LASER_FILETYPES_CHECKTYPES_H
#define LASER_FILETYPES_CHECKTYPES_H

#include <stddef.h>

struct laser_file_signature
{
    unsigned char *signature;
    size_t signature_size;
};

int laser_checktype(const char *filename,
                    const struct laser_file_signature formats[]);

extern const struct laser_file_signature laser_archiveformats[];
extern const struct laser_file_signature laser_mediaformats[];
extern const struct laser_file_signature laser_documentformats[];

#endif
