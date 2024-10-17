#include "include/utils.h"
#include <stdio.h>

laser_opts laser_utils_parsecmd(int argc, char **argv)
{
    int show_all = 0;
    int show_files = -1;
    int show_directories = -1;
    int show_symlinks = -1;
    int show_recursive = 0;
    char *dir = ".";

    int opt;

    struct option long_args[] = {
        {"all", 0, 0, 'a'},         {"Files", 0, 0, 'F'},
        {"Directories", 0, 0, 'D'}, {"Symlinks", 0, 0, 'S'},
        {"recursive", 0, 0, 'r'},   {0, 0, 0, 0}};

    while ((opt = getopt_long(argc, argv, "aFDSr", long_args, NULL)) != -1)
    {
        switch (opt)
        {
            case 'a':
                show_all = 1;
                break;
            case 'F':
                show_directories = show_directories <= 0 ? 0 : 1;
                show_symlinks = show_symlinks <= 0 ? 0 : 1;
                show_files = 1;
                break;
            case 'D':
                show_symlinks = show_symlinks <= 0 ? 0 : 1;
                show_files = show_files <= 0 ? 0 : 1;
                show_directories = 1;
                break;
            case 'S':
                show_files = show_files <= 0 ? 0 : 1;
                show_directories = show_directories <= 0 ? 0 : 1;
                show_symlinks = 1;
                break;
            case 'r':
                show_recursive = 1;
                break;
            default:
                exit(1);
        }
    }

    if (optind < argc)
        dir = argv[optind];

    return (laser_opts){show_all,      show_files,     show_directories,
                        show_symlinks, show_recursive, dir};
}

void laser_utils_format_date(time_t time, char *buffer, size_t buffer_size)
{
    struct tm *tm_info = localtime(&time);
    strftime(buffer, buffer_size, "%d-%m-%Y", tm_info);
}

char **laser_utils_grow_strarray(char **array, size_t *alloc_size, size_t count)
{
    if (count == *alloc_size)
    {
        *alloc_size *= 2;
        array = realloc(array, (*alloc_size) * sizeof(char *));
        if (array == NULL)
        {
            perror("realloc");
            exit(EXIT_FAILURE);
        }
    }
    return array;
}
