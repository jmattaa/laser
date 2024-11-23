#include "utils.h"
#include <stdio.h>

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

int laser_cmp_string(const void *a, const void *b, const void *arg)
{
    return strcmp(*(const char **)a, *(const char **)b);
}

void laser_swap(void *a, void *b, size_t size)
{
    if (a == b || size == 0)
        return;

    if (size <= 16)
    {
        char *pa = (char *)a, *pb = (char *)b;
        while (size--)
        {
            *pa ^= *pb;
            *pb ^= *pa;
            *pa ^= *pb;
            pa++;
            pb++;
        }
        return;
    }

    char temp[size];
    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);
}

int laser_charcmp(const char *str1, const char *str2)
{
    if (*str1 == '\0' || *str2 == '\0')
        return (*str1 == '\0') - (*str2 == '\0');

    return *str1 - *str2;
}

int laser_is_filestat_exec(const struct stat *file_stat)
{
    return (file_stat->st_mode & S_IXUSR) || (file_stat->st_mode & S_IXGRP) ||
           (file_stat->st_mode & S_IXOTH);
}
