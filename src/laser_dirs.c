#include "include/laser.h"

laser_dir *laser_init_dir(char *name)
{
    laser_dir *dir = malloc(sizeof(laser_dir));

    dir->name = strdup(name);

    return dir;
}

laser_dir **laser_grow_dirarray(laser_dir **dirs, size_t *alloc_size,
                                size_t count)
{
    if (count == *alloc_size)
    {
        *alloc_size *= 2;
        dirs = realloc(dirs, (*alloc_size) * sizeof(laser_dir *));
        if (dirs == NULL)
        {
            free(dirs);
            perror("realloc");
            exit(EXIT_FAILURE);
        }
    }
    return dirs;
}

void laser_free_dir(laser_dir *dir)
{
    if (dir->name)
        free(dir->name);
    if (dir)
        free(dir);
}
