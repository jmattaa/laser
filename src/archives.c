#include "include/archives.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

const struct laser_archivefomrat laser_archiveformats[] = {
    {(unsigned char[]){0x50, 0x4B, 0x03, 0x04}, 4},      // ZIP
    {(unsigned char[]){0x1F, 0x8B}, 2},                  // GZIP
    {(unsigned char[]){'u', 's', 't', 'a', 'r'}, 5},     // TAR
    {(unsigned char[]){0x42, 0x5A, 0x68}, 3},            // BZIP2
    {(unsigned char[]){0xFD, 0x37, 0x7A, 0x58, 0x5A}, 5} // XZ
    // ADD MORE ARCHIVES
};

int laser_is_archive(const char *filename)
{
    size_t num_formats =
        sizeof(laser_archiveformats) / sizeof(laser_archiveformats[0]);

    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("open");
        return 0;
    }

    // should be updated if there is a magic number greater than 8 bits
    unsigned char buffer[8];

    ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
    close(fd);

    if (bytesRead < 0)
    {
        fprintf(stderr, "laser: cannot read %s", filename);
        return 0;
    }

    for (size_t i = 0; i < num_formats; i++)
    {
        if (bytesRead >= laser_archiveformats[i].magic_size &&
            memcmp(buffer, laser_archiveformats[i].magic,
                   laser_archiveformats[i].magic_size) == 0)
            return 1;
    }

    return 0;
}
