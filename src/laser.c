#include "include/laser.h"
#include "include/colors.h"
#include "include/utils.h"
#include <stdio.h>

laser_dir_entries laser_getdirs(laser_opts opts)
{
    DIR *dir_stuff = opendir(opts.dir); // idk what to name this

    laser_dir_entries entries = {0};

    if (dir_stuff == NULL)
    {
        perror("opendir");
        return entries;
    }

    struct stat file_stat;
    struct dirent *entry;

    char full_path[1024];
    char symlink_target[1024];

    size_t dir_alloc = 10;
    size_t symlink_alloc = 10;
    size_t hidden_alloc = 10;
    size_t file_alloc = 10;

    while ((entry = readdir(dir_stuff)) != NULL)
    {
        if (!opts.show_all && entry->d_name[0] == '.')
            continue;

        snprintf(full_path, sizeof(full_path), "%s/%s", opts.dir,
                 entry->d_name);

        if (lstat(full_path, &file_stat) == -1)
        {
            perror("lstat");
            continue;
        }

        if (S_ISDIR(file_stat.st_mode) && opts.show_directories)
        {
            if (entries.dir_count == 0)
                entries.dirs = malloc(dir_alloc * sizeof(laser_dir *));

            entries.dirs = laser_grow_dirarray(entries.dirs, &dir_alloc,
                                               entries.dir_count);
            entries.dirs[entries.dir_count] = laser_init_dir(entry->d_name);

            if (opts.show_recursive && strcmp(entry->d_name, ".") == 0 &&
                strcmp(entry->d_name, "..") == 0)
            {
                laser_opts sub_opts = opts;
                sub_opts.dir = full_path;

                laser_dir_entries subentries = laser_getdirs(sub_opts);
                entries.dirs[entries.dir_count]->sub_entires = &subentries;
            }

            entries.dir_count++;
        }
        else if (S_ISLNK(file_stat.st_mode) && opts.show_symlinks)
        {
            if (entries.symlink_count == 0)
                entries.symlinks = malloc(symlink_alloc * sizeof(char *));

            int len =
                readlink(full_path, symlink_target, sizeof(symlink_target) - 1);
            if (len == -1)
            {
                perror("readlink");
                continue;
            }
            symlink_target[len] = '\0';

            char *arrow = " -> ";

            size_t res_string_len =
                len + strlen(entry->d_name) + strlen(arrow) + 1;
            char res_string[res_string_len];

            snprintf(res_string, res_string_len, "%s%s%s", entry->d_name, arrow,
                     symlink_target);

            entries.symlinks = laser_utils_grow_strarray(
                entries.symlinks, &symlink_alloc, entries.symlink_count);
            entries.symlinks[entries.symlink_count] = strdup(res_string);

            entries.symlink_count++;
        }
        else if (entry->d_name[0] == '.')
        {
            if (entries.hidden_count == 0)
                entries.hidden = malloc(hidden_alloc * sizeof(char *));

            entries.hidden = laser_utils_grow_strarray(
                entries.hidden, &hidden_alloc, entries.hidden_count);
            entries.hidden[entries.hidden_count] = strdup(entry->d_name);

            entries.hidden_count++;
        }
        else if (S_ISREG(file_stat.st_mode) && opts.show_files)
        {
            if (entries.file_count == 0)
                entries.files = malloc(file_alloc * sizeof(char *));

            entries.files = laser_utils_grow_strarray(
                entries.files, &file_alloc, entries.file_count);
            entries.files[entries.file_count] = strdup(entry->d_name);

            entries.file_count++;
        }
    }

    closedir(dir_stuff);
    return entries;
}

int laser_cmp_string(const void *a, const void *b)
{
    return strcmp(*(const char **)a, *(const char **)b);
}

void laser_list(laser_dir_entries lentries)
{
    qsort(lentries.files, lentries.file_count, sizeof(char *),
          laser_cmp_string);
    qsort(lentries.hidden, lentries.hidden_count, sizeof(char *),
          laser_cmp_string);
    qsort(lentries.symlinks, lentries.symlink_count, sizeof(char *),
          laser_cmp_string);

    if (lentries.dir_count > 0)
    {
        for (int i = 0; i < lentries.dir_count; i++)
        {
            printf(DIR_COLOR "%s/" RESET_COLOR "\n", lentries.dirs[i]->name);
            laser_free_dir(lentries.dirs[i]);
        }

        free(lentries.dirs);
        printf("\n");
    }

    if (lentries.file_count > 0)
    {
        for (int i = 0; i < lentries.file_count; i++)
        {
            printf(FILE_COLOR "%s" RESET_COLOR "\n", lentries.files[i]);
            free(lentries.files[i]);
        }

        free(lentries.files);
        printf("\n");
    }

    if (lentries.hidden_count > 0)
    {
        for (int i = 0; i < lentries.hidden_count; i++)
        {
            printf(HIDDEN_COLOR "%s" RESET_COLOR "\n", lentries.hidden[i]);
            free(lentries.hidden[i]);
        }

        free(lentries.hidden);
        printf("\n");
    }

    if (lentries.symlink_count > 0)
    {
        for (int i = 0; i < lentries.symlink_count; i++)
        {
            printf(SYMLINK_COLOR "%s" RESET_COLOR "\n", lentries.symlinks[i]);
            free(lentries.symlinks[i]);
        }

        free(lentries.symlinks);
        printf("\n");
    }
}
