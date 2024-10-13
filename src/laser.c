#include "include/laser.h"
#include "include/colors.h"
#include "include/utils.h"

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

        if (S_ISDIR(file_stat.st_mode))
            entries.dirs[entries.dir_count++] = strdup(entry->d_name);
        else if (S_ISLNK(file_stat.st_mode))
            entries.symlinks[entries.symlink_count++] = strdup(entry->d_name);
        else if (entry->d_name[0] == '.')
            entries.hidden[entries.hidden_count++] = strdup(entry->d_name);
        else
            entries.files[entries.file_count++] = strdup(entry->d_name);
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
    qsort(lentries.dirs, lentries.dir_count, sizeof(char *), laser_cmp_string);
    qsort(lentries.files, lentries.file_count, sizeof(char *),
          laser_cmp_string);
    qsort(lentries.hidden, lentries.hidden_count, sizeof(char *),
          laser_cmp_string);
    qsort(lentries.symlinks, lentries.symlink_count, sizeof(char *),
          laser_cmp_string);

    if (lentries.dir_count > 0)
    {
        char full_path[1024];
        for (int i = 0; i < lentries.dir_count; i++)
        {
            printf(DIR_COLOR "%s/" RESET_COLOR "\n", lentries.dirs[i]);
            free(lentries.dirs[i]);
        }

        printf("\n");
    }

    if (lentries.file_count > 0)
    {
        for (int i = 0; i < lentries.file_count; i++)
        {
            printf(FILE_COLOR "%s" RESET_COLOR "\n", lentries.files[i]);
            free(lentries.files[i]);
        }

        printf("\n");
    }

    if (lentries.hidden_count > 0)
    {
        for (int i = 0; i < lentries.hidden_count; i++)
        {
            printf(HIDDEN_COLOR "%s" RESET_COLOR "\n", lentries.hidden[i]);
            free(lentries.hidden[i]);
        }

        printf("\n");
    }

    if (lentries.symlink_count > 0)
    {
        for (int i = 0; i < lentries.symlink_count; i++)
        {
            printf(SYMLINK_COLOR "%s" RESET_COLOR "\n", lentries.symlinks[i]);
            free(lentries.symlinks[i]);
        }

        printf("\n");
    }
}
