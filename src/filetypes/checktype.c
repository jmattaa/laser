#include "filetypes/checktypes.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

enum check_type_return
{
    Success = 0,
    NotOpened,
    CannotRead,
    Unknown
};
static int laser_checktype_fd(int fd, const struct laser_magicnumber formats[])
{
    off_t curr_pos = lseek(fd, 0, SEEK_CUR);
    if (fd == -1)
    {
        return NotOpened;
    }
    lseek(fd, 0, SEEK_SET); // seek to start
    // should be updated if there is a magic number larger than 8 bits
    unsigned char buffer[8];

    ssize_t bytesRead = read(fd, buffer, sizeof(buffer));

    if (bytesRead < 0)
    {
        lseek(fd, curr_pos, SEEK_SET);
        return CannotRead;
    }

    size_t i = 0;

    while (formats[i].magic_size != 0)
    {
        if (bytesRead >= formats[i].magic_size &&
            memcmp(buffer, formats[i].magic, formats[i].magic_size) == 0)
        {
            lseek(fd, curr_pos, SEEK_SET);
            return Success;
        }
        i++;
    }
    lseek(fd, curr_pos, SEEK_SET);
    return Unknown;
}

int laser_checktype(const char *filename,
                    const struct laser_magicnumber formats[])
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("open");
        return 0;
    }
    int rv = laser_checktype_fd(fd, formats);
    // should be updated if there is a magic number larger than 8 bits
    switch (rv)
    {
        case NotOpened:
            perror("open");
            break;
        case CannotRead:
            fprintf(stderr, "laser: cannot read %s", filename);
            break;
            //Fallthrough
        default:
            break;
    }
    close(fd);
    return rv == Success;
}

int laser_checktype_ex(int fd, const char *filename,
                       const struct laser_magicnumber formats[])
{

    if (fd == -1)
    {
        perror("open");
        return 0;
    }
    int rv = laser_checktype_fd(fd, formats);
    // should be updated if there is a magic number larger than 8 bits
    switch (rv)
    {
        case NotOpened:
            perror("open");
            break;
        case CannotRead:
            fprintf(stderr, "laser: cannot read %s", filename);
            break;
            //Fallthrough
        default:
            break;
    }
    return rv == Success;
}
