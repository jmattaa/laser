#include "filetypes/checktypes.h"
#include "logger.h"

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

int laser_checktype(const char *filename, const struct laser_filetype formats[])
{
    if (laser_checktype_extension(filename, formats) == Success)
        return 1;

    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("open");
        return 0;
    }

    int rv = laser_checktype_magic(fd, formats);
    switch (rv)
    {
        case NotOpened:
            perror("open");
            break;
        case CannotRead:
            laser_logger_error("cannot read %s", filename);
            break;
        default:
            break;
    }
    close(fd);
    return rv == Success;
}

int laser_checktype_extension(const char *filename,
                              const struct laser_filetype *formats)
{
    int rv = Unknown;

    if (filename[0] == '.') // if a hidden file is hidden then it's hidden
                            // u see my wisdom and my big brain
        return rv;

    char *ext = strrchr(filename, '.');
    if (ext == NULL)
        return rv;

    ext++; // skip the .

    for (int i = 0; formats[i].extensions != NULL; i++)
        for (size_t j = 0; j < formats[i].extensions_count; j++)
            if (strcmp(ext, formats[i].extensions[j]) == 0)
                return Success;

    return rv;
}

int laser_checktype_magic(int fd, const struct laser_filetype formats[])
{

    if (fd == -1)
    {
        perror("open");
        return 0;
    }

    off_t curr_pos = lseek(fd, 0, SEEK_CUR);
    if (fd == -1)
    {
        return NotOpened;
    }
    lseek(fd, 0, SEEK_SET); // seek to start

    // ----------------------------------------------------------------------
    // should be updated if there is a magic number larger than 8 bits
    unsigned char buffer[8];

    long bytesRead = read(fd, buffer, sizeof(buffer));

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
