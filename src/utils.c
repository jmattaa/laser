#include "include/utils.h"
#include <stdio.h>

laser_opts laser_utils_parsecmd(int argc, char **argv)
{
    int show_all = 0;
    int show_files = -1;
    int show_directories = -1;
    int show_symlinks = -1;
    int show_git = 0;
    int show_tree = 0;
    char *dir = ".";

    int opt;

    struct option long_args[] = {{"all", 0, 0, 'a'},
                                 {"Files", 0, 0, 'F'},
                                 {"Directories", 0, 0, 'D'},
                                 {"Symlinks", 0, 0, 'S'},
                                 {"Git", 0, 0, 'G'},
                                 {"recursive", 0, 0, 'r'},
                                 {0, 0, 0, 0}};

    while ((opt = getopt_long(argc, argv, "aFDSGr", long_args, NULL)) != -1)
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
            case 'G':
                show_git = 1;
                break;
            case 'r':
                show_tree = 1;
                // recursive listing has to ovveride dir flag
                show_directories = 1;
                break;
            default:
                exit(1);
        }
    }

    if (optind < argc)
        dir = argv[optind];

    return (laser_opts){show_all,      show_files,      show_directories,
                        show_symlinks, show_git,        show_tree,
                        dir,           .parentDir = dir};
}

void laser_utils_format_date(time_t time, char *buffer, size_t buffer_size)
{
    struct tm *tm_info = localtime(&time);
    strftime(buffer, buffer_size, "%d-%m-%Y", tm_info);
}

int laser_cmp_string_for_gitignore(const void *a, const void *b)
{
    const char *target = *(const char **)a;
    const char *pattern = *(const char **)b;

    size_t pattern_len = strlen(pattern);

    if (pattern[pattern_len - 1] == '/')
    {
        if (strncmp(target, pattern, pattern_len - 1) == 0)
            return 0;
    }
    else
        return strcmp(target, pattern);

    return strcmp(target, pattern);
}
int laser_string_in_sorted_array(char *target, char **array, int size)
{
    char **item = (char **)bsearch(&target, array, size, sizeof(char *),
                                   laser_cmp_string_for_gitignore);
    return item != NULL;
}

char **laser_utils_grow_strarray(char **array, size_t *alloc_size, size_t count)
{
    if (count == *alloc_size)
    {
        *alloc_size *= 2;
        array = realloc(array, (*alloc_size) * sizeof(char *));
        if (array == NULL)
        {
            free(array);
            perror("realloc");
            exit(EXIT_FAILURE);
        }
    }
    return array;
}

int laser_cmp_string(const void *a, const void *b)
{
    return strcmp(*(const char **)a, *(const char **)b);
}
