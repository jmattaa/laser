#include "laser.h"
#include "filetypes/checktypes.h"
#include "git/lgit.h"
#include "utils.h"

char *strip_parent_dir(const char *full_path, const char *parent_dir)
{
    size_t parent_len = strlen(parent_dir);

    if (strncmp(full_path, parent_dir, parent_len) == 0 &&
        full_path[parent_len] == '/')
    {
        return (char *)(full_path + parent_len + 1);
    }

    return (char *)full_path;
}

int laser_cmp_dirent(const void *a, const void *b, const void *arg)
{
    const char *dir_path = (char *)arg;
    struct dirent *dirent_a = *(struct dirent **)a;
    struct dirent *dirent_b = *(struct dirent **)b;

    char path_a[1024];
    char path_b[1024];
    snprintf(path_a, sizeof(path_a), "%s/%s", dir_path, dirent_a->d_name);
    snprintf(path_b, sizeof(path_b), "%s/%s", dir_path, dirent_b->d_name);

    struct stat stat_a, stat_b;

    lstat(path_a, &stat_a);
    lstat(path_b, &stat_b);

    // // add weight if is file so files go down, u see gravity (me big brain)
    int weight = 0; // idk what to call this
    if (S_ISDIR(stat_a.st_mode) && !S_ISDIR(stat_b.st_mode))
        weight = -1;
    else if (!S_ISDIR(stat_a.st_mode) && S_ISDIR(stat_b.st_mode))
        weight = 1; // weigh more --> fall down

    if (weight == 0) // they weigh the same so compare the name
        return laser_charcmp(dirent_a->d_name, dirent_b->d_name);

    return weight;
}

void laser_print_entry(const char *name, const char *color, char *indent,
                       int depth, int is_last)
{
    char branch[8] = "";
    if (depth > 0)
        strcpy(branch, is_last ? "└─ " : "├─ ");

    printf("%s%s%s%s" RESET_COLOR "\n", indent, branch, color, name);
}

void laser_process_entries(laser_opts opts, int depth, char *indent,
                           char **gitignore_patterns, int gitignore_count)
{
    DIR *dir = opendir(opts.dir);
    if (dir == NULL)
    {
        fprintf(stderr, "lsr: couldn't open %s, %s\n", opts.dir,
                strerror(errno));
        return;
    }

    struct dirent *entry;
    struct dirent **entries = malloc(sizeof(struct dirent *));
    int entry_count = 0;

    char full_path[1024];
    while ((entry = readdir(dir)) != NULL)
    {
        snprintf(full_path, sizeof(full_path), "%s/%s", opts.dir,
                 entry->d_name);

        struct stat file_stat;
        if (lstat(full_path, &file_stat) == -1)
        {
            fprintf(stderr, "lsr: %s\n", strerror(errno));
            continue;
        }

        if (!opts.show_all && entry->d_name[0] == '.')
            continue;

        if (opts.show_git && (laser_string_in_sorted_array(
                                  strip_parent_dir(full_path, opts.parentDir),
                                  gitignore_patterns, gitignore_count) ||
                              strcmp(entry->d_name, ".git") == 0))
            continue;

        if ((S_ISDIR(file_stat.st_mode) && opts.show_directories) ||
            (S_ISLNK(file_stat.st_mode) && opts.show_symlinks) ||
            (S_ISREG(file_stat.st_mode) && opts.show_files))
        {
            if (opts.show_tree && S_ISDIR(file_stat.st_mode) &&
                (strcmp(entry->d_name, ".") == 0 ||
                 strcmp(entry->d_name, "..") == 0))
                continue;

            entries =
                realloc(entries, (entry_count + 1) * sizeof(struct dirent *));

            size_t entry_size =
                offsetof(struct dirent, d_name) + strlen(entry->d_name) + 1;
            entries[entry_count] = malloc(entry_size);
            memcpy(entries[entry_count], entry, entry_size);
            entry_count++;
        }
    }
    // sort and print stuff
    laser_sort(entries, entry_count, sizeof(struct dirent *), laser_cmp_dirent,
               opts.parentDir);
    for (int i = 0; i < entry_count; i++)
    {
        int is_last = (i == entry_count - 1);
        struct stat file_stat;
        snprintf(full_path, sizeof(full_path), "%s/%s", opts.dir,
                 entries[i]->d_name);
        lstat(full_path, &file_stat);

        if (S_ISDIR(file_stat.st_mode))
        {
            laser_print_entry(entries[i]->d_name, DIR_COLOR, indent, depth,
                              is_last);

            if (opts.show_tree)
            {
                laser_opts sub_opts = opts;
                sub_opts.dir = full_path;
                laser_list_directory(sub_opts, depth + 1);
            }
        }
        else
        {
            if (S_ISLNK(file_stat.st_mode) && opts.show_symlinks)
            {
                char symlink_target[1024];
                int len = readlink(full_path, symlink_target,
                                   sizeof(symlink_target) - 1);
                if (len != -1)
                {
                    symlink_target[len] = '\0';
                    char res_string[2048];
                    snprintf(res_string, sizeof(res_string), "%s -> %s",
                             entries[i]->d_name, symlink_target);
                    laser_print_entry(res_string, SYMLINK_COLOR, indent, depth,
                                      is_last);
                }
            }
            else if (laser_is_filestat_exec(&file_stat))
            {
                laser_print_entry(entries[i]->d_name, EXEC_COLOR, indent, depth,
                                  is_last);
            }
            else if (laser_checktype(full_path, laser_archiveformats))
            {
                laser_print_entry(entries[i]->d_name, ARCHIVE_COLOR, indent,
                                  depth, is_last);
            }
            else if (laser_checktype(full_path, laser_mediaformats))
            {
                laser_print_entry(entries[i]->d_name, MEDIA_COLOR, indent,
                                  depth, is_last);
            }
            else if (entries[i]->d_name[0] == '.')
            {
                laser_print_entry(entries[i]->d_name, HIDDEN_COLOR, indent,
                                  depth, is_last);
            }
            else if (S_ISREG(file_stat.st_mode))
            {
                laser_print_entry(entries[i]->d_name, FILE_COLOR, indent, depth,
                                  is_last);
            }
        }

        free(entries[i]);
    }
    free(entries);
    closedir(dir);
}

void laser_list_directory(laser_opts opts, int depth)
{
    const char *pipe = "│   ";
    size_t indent_len = depth > 0 ? depth * strlen(pipe) : 0;
    char *indent = malloc(indent_len + 1);
    indent[0] = '\0';
    if (depth > 0)
        for (int i = 1; i < depth; i++)
            strcat(indent, pipe);

    int gitignore_count = 0;
    char **gitignore_patterns = NULL;
    if (opts.show_git)
    {
        gitignore_patterns =
            lgit_parseGitignore(opts.parentDir, &gitignore_count);
        laser_sort(gitignore_patterns, gitignore_count, sizeof(char *),
                   laser_cmp_string, NULL);
    }

    laser_process_entries(opts, depth, indent, gitignore_patterns,
                          gitignore_count);
    free(indent);

    if (gitignore_patterns)
    {
        for (int i = 0; i < gitignore_count; i++)
        {
            free(gitignore_patterns[i]);
        }
        free(gitignore_patterns);
    }
}
