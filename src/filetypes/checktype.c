#include "filetypes/checktypes.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int laser_checktype(const char *filename,
                    const struct laser_file_signature formats[])
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("open");
        return 0;
    }

    // should be updated if there is a magic number larger than 8 bits
    unsigned char buffer[8];

    ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
    close(fd);

    if (bytesRead < 0)
    {
        fprintf(stderr, "laser: cannot read %s", filename);
        return 0;
    }

    size_t i = 0;

    while (formats[i].signature_size != 0)
    {
        if (bytesRead >= formats[i].signature_size &&
            memcmp(buffer, formats[i].signature, formats[i].signature_size) ==
                0)
            return 1;
        i++;
    }

    return 0;
}
