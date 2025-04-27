#ifndef LASER_UTILS_H
#define LASER_UTILS_H

#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

char **laser_utils_grow_strarray(char **array, size_t *alloc_size,
                                 size_t count);
int laser_string_in_sorted_array(char *target, char **array, int size);
void laser_swap(void *a, void *b, size_t size);
int laser_charcmp(const char *a, const char *b);

int laser_is_filestat_exec(const struct stat *file_stat);

#endif
