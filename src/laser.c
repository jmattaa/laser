#include "laser.h"
#include "colors.h"
#include "filetypes/checktypes.h"
#include "git/lgit.h"
#include "utils.h"
#include <fcntl.h>

#define BRANCH_SIZE 8

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
    struct laser_dirent *dirent_a = *(struct laser_dirent **)a;
    struct laser_dirent *dirent_b = *(struct laser_dirent **)b;

    // add weight if is file so files go down, u see gravity (me big brain)
    int weight = 0; // idk what to call this
    if (S_ISDIR(dirent_a->s.st_mode) && !S_ISDIR(dirent_b->s.st_mode))
        weight = -1;
    else if (!S_ISDIR(dirent_a->s.st_mode) && S_ISDIR(dirent_b->s.st_mode))
        weight = 1; // weigh more --> fall down

    if (weight == 0) // they weigh the same so compare the name
        return laser_charcmp(dirent_a->d->d_name, dirent_b->d->d_name);

    return weight;
}

void laser_print_long_entry(struct laser_dirent *entry, const char *color,
                            char *indent, const char *branch)
{
    char perms[11];
    perms[0] = (S_ISDIR(entry->s.st_mode))    ? 'd'
               : (S_ISLNK(entry->s.st_mode))  ? 'l'
               : (S_ISCHR(entry->s.st_mode))  ? 'c'
               : (S_ISBLK(entry->s.st_mode))  ? 'b'
               : (S_ISFIFO(entry->s.st_mode)) ? 'p'
               : (S_ISSOCK(entry->s.st_mode)) ? 's'
                                              : '-'; // regular file

    perms[1] = (entry->s.st_mode & S_IRUSR) ? 'r' : '-';
    perms[2] = (entry->s.st_mode & S_IWUSR) ? 'w' : '-';
    perms[3] = (entry->s.st_mode & S_IXUSR) ? 'x' : '-';
    perms[4] = (entry->s.st_mode & S_IRGRP) ? 'r' : '-';
    perms[5] = (entry->s.st_mode & S_IWGRP) ? 'w' : '-';
    perms[6] = (entry->s.st_mode & S_IXGRP) ? 'x' : '-';
    perms[7] = (entry->s.st_mode & S_IROTH) ? 'r' : '-';
    perms[8] = (entry->s.st_mode & S_IWOTH) ? 'w' : '-';
    perms[9] = (entry->s.st_mode & S_IXOTH) ? 'x' : '-';
    perms[10] = '\0'; // Ensure null termination

    char mtime[20];
    struct tm *tm_info = localtime(&entry->s.st_mtime);
    strftime(mtime, sizeof(mtime), "%b %e %H:%M", tm_info);

    char size[6];
    if (S_ISDIR(entry->s.st_mode) || S_ISLNK(entry->s.st_mode))
        snprintf(size, sizeof(size), "%5s", "-");
    else if (entry->s.st_size < 1000)
        snprintf(size, sizeof(size), "%5d", (int)entry->s.st_size);
    else if (entry->s.st_size < 1000000)
        snprintf(size, sizeof(size), "%4dK", (int)(entry->s.st_size / 1000));
    else if (entry->s.st_size < 1000000000)
        snprintf(size, sizeof(size), "%4dM", (int)(entry->s.st_size / 1000000));
    else
        snprintf(size, sizeof(size), "%4dG",
                 (int)(entry->s.st_size / 1000000000));

    char *user = getpwuid(entry->s.st_uid)->pw_name;

    printf("%s  %s%s%s %s%s%s %s%s%s  %s%s%s%s%s\n", perms,
           LASER_COLORS_DEFAULTS[LASER_COLOR_SYMLINK].value, mtime,
           LASER_COLORS_DEFAULTS[LASER_COLOR_RESET].value,
           LASER_COLORS_DEFAULTS[LASER_COLOR_MEDIA].value, size,
           LASER_COLORS_DEFAULTS[LASER_COLOR_RESET].value,
           LASER_COLORS_DEFAULTS[LASER_COLOR_SYMLINK].value, user,
           LASER_COLORS_DEFAULTS[LASER_COLOR_RESET].value, indent, branch,
           color, entry->d->d_name, LASER_COLORS[LASER_COLOR_RESET].value);
}

void laser_print_entry(struct laser_dirent *entry, const char *color,
                       char *indent, int depth, laser_opts opts, int is_last)
{
    char branch[BRANCH_SIZE] = "";
    if (depth > 0)
        strcpy(branch, is_last ? "└─ " : "├─ ");

    if (opts.show_long)
        laser_print_long_entry(entry, color, indent, branch);
    else
        printf("%s%s%s%s%s\n", indent, branch, color, entry->d->d_name,
               LASER_COLORS[LASER_COLOR_RESET].value);
}

static laser_color_type laser_color_for_format(const char *filename)
{
    if (laser_checktype(filename, laser_archiveformats))
        return LASER_COLOR_ARCHIVE;
    else if (laser_checktype(filename, laser_mediaformats))
        return LASER_COLOR_MEDIA;
    else if (laser_checktype(filename, laser_documentformats))
        return LASER_COLOR_DOCUMENT;

    return LASER_COLOR_FILE;
}

void laser_process_entries(laser_opts opts, int depth, int max_depth,
                           char *indent, char **gitignore_patterns,
                           int gitignore_count)
{
    DIR *dir = opendir(opts.dir);
    if (dir == NULL)
    {
        fprintf(stderr, "lsr: couldn't open %s, %s\n", opts.dir,
                strerror(errno));
        return;
    }

    struct laser_dirent *entry = malloc(sizeof(struct laser_dirent));
    struct laser_dirent **entries = malloc(sizeof(struct laser_dirent *));
    int entry_count = 0;

    char full_path[LASER_PATH_MAX];
    while ((entry->d = readdir(dir)) != NULL)
    {
        snprintf(full_path, sizeof(full_path), "%s/%s", opts.dir,
                 entry->d->d_name);

        if (lstat(full_path, &entry->s) == -1)
        {
            fprintf(stderr, "lsr: %s\n", strerror(errno));
            continue;
        }

        if (!opts.show_all && entry->d->d_name[0] == '.')
            continue;

        if (opts.show_git && (laser_string_in_sorted_array(
                                  strip_parent_dir(full_path, opts.parentDir),
                                  gitignore_patterns, gitignore_count) ||
                              strcmp(entry->d->d_name, ".git") == 0))
            continue;

        if ((S_ISDIR(entry->s.st_mode) && opts.show_directories) ||
            (S_ISLNK(entry->s.st_mode) && opts.show_symlinks) ||
            (S_ISREG(entry->s.st_mode) && opts.show_files))
        {
            if (opts.show_tree && S_ISDIR(entry->s.st_mode) &&
                (strcmp(entry->d->d_name, ".") == 0 ||
                 strcmp(entry->d->d_name, "..") == 0))
                continue;

            entries = realloc(entries, (entry_count + 1) *
                                           sizeof(struct laser_dirent *));

            size_t entry_size = sizeof(struct laser_dirent) +
                                offsetof(struct dirent, d_name) +
                                strlen(entry->d->d_name) + 1;

            entries[entry_count] = malloc(entry_size);

            entries[entry_count]->s = entry->s;
            entries[entry_count]->d = malloc(offsetof(struct dirent, d_name) +
                                             strlen(entry->d->d_name) + 1);

            memcpy(entries[entry_count]->d, entry->d,
                   offsetof(struct dirent, d_name) + strlen(entry->d->d_name) +
                       1);

            entry_count++;
        }
    }
    free(entry); // entry is no longer needed it's been copied to entries
                 // u see my dumbass created mem leaks

    // sort and print stuff
    laser_sort(entries, entry_count, sizeof(struct laser_dirent *),
               laser_cmp_dirent, NULL);
    for (int i = 0; i < entry_count; i++)
    {
        int is_last = (i == entry_count - 1);

        snprintf(full_path, sizeof(full_path), "%s/%s", opts.dir,
                 entries[i]->d->d_name);

        if (S_ISDIR(entries[i]->s.st_mode))
        {
            laser_print_entry(entries[i], LASER_COLORS[LASER_COLOR_DIR].value,
                              indent, depth, opts, is_last);

            if (opts.show_tree)
            {
                laser_opts sub_opts = opts;
                sub_opts.dir = full_path;
                laser_list_directory(sub_opts, depth + 1, max_depth);
            }
        }
        else
        {

            if (S_ISLNK(entries[i]->s.st_mode) && opts.show_symlinks)
            {
                char symlink_target[LASER_PATH_MAX];
                int len = readlink(full_path, symlink_target,
                                   sizeof(symlink_target) - 1);
                if (len != -1)
                {
                    symlink_target[len] = '\0';

                    char res_string[LASER_PATH_MAX * 2 + 4]; // 4 is " -> "

                    snprintf(res_string, sizeof(res_string), "%s -> %s",
                             entries[i]->d->d_name, symlink_target);

                    struct laser_dirent *ent =
                        malloc(sizeof(struct laser_dirent));

                    ent->s = entries[i]->s;
                    ent->d = malloc(offsetof(struct dirent, d_name) +
                                    strlen(res_string) + 1);

                    strcpy(ent->d->d_name, res_string);

                    laser_print_entry(ent,
                                      LASER_COLORS[LASER_COLOR_SYMLINK].value,
                                      indent, depth, opts, is_last);

                    free(ent->d);
                    free(ent);
                }
            }
            else if (laser_is_filestat_exec(&entries[i]->s))
            {
                laser_print_entry(entries[i],
                                  LASER_COLORS[LASER_COLOR_EXEC].value, indent,
                                  depth, opts, is_last);
            }
            else if (entries[i]->d->d_name[0] == '.')
            {
                laser_print_entry(entries[i],
                                  LASER_COLORS[LASER_COLOR_HIDDEN].value,
                                  indent, depth, opts, is_last);
            }

            else
            {
                // coloring which depends on formats
                laser_color_type color_type = laser_color_for_format(full_path);
                if (color_type != LASER_COLOR_FILE)
                {
                    laser_print_entry(entries[i],
                                      LASER_COLORS[color_type].value, indent,
                                      depth, opts, is_last);
                }
                else if (S_ISREG(entries[i]->s.st_mode))
                    laser_print_entry(entries[i],
                                      LASER_COLORS[LASER_COLOR_FILE].value,
                                      indent, depth, opts, is_last);
            }
        }

        free(entries[i]->d);
        free(entries[i]);
    }
    free(entries);
    closedir(dir);
}

void laser_list_directory(laser_opts opts, int depth, int max_depth)
{
    if (max_depth >= 0 && depth > max_depth)
        return;

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

    laser_process_entries(opts, depth, max_depth, indent, gitignore_patterns,
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
