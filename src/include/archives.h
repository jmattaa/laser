#ifndef LASER_ARCHIVES_H
#define LASER_ARCHIVES_H

#include <stddef.h>

struct laser_archivefomrat
{
    unsigned char *magic;
    size_t magic_size;
};

extern const struct laser_archivefomrat laser_archiveformats[];

int laser_is_archive(const char *filename);

#endif
