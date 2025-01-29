#include "laser.h"
#include "colors.h"
#include "filetypes/checktypes.h"
#include "git/lgit.h"
#include "init_lua.h"
#include "lua_filters.h"
#include "utils.h"
#include <fcntl.h>
#include <lua.h>
#include <pwd.h>
#include <sys/stat.h>

#define BRANCH_SIZE 8

static ssize_t longest_ownername = 0;

static int laser_cmp_dirent(const void *a, const void *b, const void *arg)
{
    struct laser_dirent *dirent_a = *(struct laser_dirent **)a;
    struct laser_dirent *dirent_b = *(struct laser_dirent **)b;

    lua_getglobal(L, "L_compare_entries");

    lua_pushstring(L, dirent_a->d->d_name);
    lua_pushstring(L, dirent_b->d->d_name);
    lua_pushboolean(L, S_ISDIR(dirent_a->s.st_mode));
    lua_pushboolean(L, S_ISDIR(dirent_b->s.st_mode));

    if (lua_pcall(L, 4, 1, 0) != LUA_OK)
    {
        fprintf(stderr, "lsr: error in L_compare_entries: %s\n",
                lua_tostring(L, -1));
        lua_pop(L, 1);
        return 0;
    }

    int result = (int)lua_tointeger(L, -1);
    lua_pop(L, 1);

    return result;
}

static void laser_print_long_entry(struct laser_dirent *entry,
                                   const char *color, char *indent,
                                   const char *branch)
{
    lua_getglobal(L, "L_long_format");

    lua_newtable(L);

    lua_pushstring(L, entry->d->d_name);
    lua_setfield(L, -2, "name");

    lua_pushinteger(L, entry->s.st_mode);
    lua_setfield(L, -2, "mode");

    off_t size = (S_ISDIR(entry->s.st_mode)) ? -1 : entry->s.st_size;
    lua_pushinteger(L, size);
    lua_setfield(L, -2, "size");

    lua_pushinteger(L, entry->s.st_mtime);
    lua_setfield(L, -2, "mtime");

    lua_pushstring(L, getpwuid(entry->s.st_uid)->pw_name);
    lua_setfield(L, -2, "owner");
    lua_pushstring(L, S_ISDIR(entry->s.st_mode)    ? "d"
                      : S_ISLNK(entry->s.st_mode)  ? "l"
                      : S_ISCHR(entry->s.st_mode)  ? "c"
                      : S_ISBLK(entry->s.st_mode)  ? "b"
                      : S_ISFIFO(entry->s.st_mode) ? "p"
                      : S_ISSOCK(entry->s.st_mode) ? "s"
                                                   : "-");
    lua_setfield(L, -2, "type");

    lua_pushstring(
        L, (char[]){entry->git_status == ' ' ? 0 : entry->git_status, 0});
    lua_setfield(L, -2, "git_status");

    lua_pushinteger(L, longest_ownername);

    if (lua_pcall(L, 2, 1, 0) != LUA_OK)
    {
        fprintf(stderr, "Error in long_format: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
        return;
    }

    const char *result = lua_tostring(L, -1);
    lua_pop(L, 1);

    printf("%s%s%s%s%s%s%s", result, LASER_COLORS[LASER_COLOR_RESET].value,
           indent, branch, color, entry->d->d_name,
           LASER_COLORS[LASER_COLOR_RESET].value);
    if (entry->git_status && entry->git_status != ' ')
        printf("\x1b[33m [%c]\x1b[0m", entry->git_status);
    printf("\n");
}

void laser_print_entry(struct laser_dirent *entry, const char *color,
                       char *indent, int depth, laser_opts opts, int is_last)
{
    char branch[BRANCH_SIZE] = "";
    if (depth > 0)
        strcpy(branch, is_last ? "└─ " : "├─ ");

    if (opts.show_long)
    {
        laser_print_long_entry(entry, color, indent, branch);
        return;
    }

    printf("%s%s%s%s%s", indent, branch, color, entry->d->d_name,
           LASER_COLORS[LASER_COLOR_RESET].value);
    if (entry->git_status && entry->git_status != ' ')
        printf("\x1b[33m [%c]\x1b[0m", entry->git_status);
    printf("\n");
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

static void laser_process_entries(laser_opts opts, int depth, int max_depth,
                                  char *indent)

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

    int entry_ignored = 0;

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

        if (opts.show_git->hide_git_ignored)
        {
            // always ignore .git
            if (strcmp(entry->d->d_name, ".git") == 0)
                continue;

            // skip the leading "./" cuz libgit dosen't like it
            if (git_ignore_path_is_ignored(&entry_ignored, opts.git_repo,
                                           strncmp(full_path, "./", 2) == 0
                                               ? &full_path[2]
                                               : full_path) < 0)
            {
                fprintf(stderr, "lsr: %s\n", git_error_last()->message);
                continue;
            }

            if (entry_ignored == 1)
                continue;
        }

        // default status to ' '
        entry->git_status = ' ';
        if (opts.show_git->show_git_status)
            lgit_getGitStatus(opts, entry, full_path);

        if (opts.show_git->show_git_status)
            lgit_getGitStatus(opts, entry, full_path);

        if (!lua_filters_apply(opts, entry))
            continue;

        if (!opts.show_all && entry->d->d_name[0] == '.')
            continue;

        if ((S_ISDIR(entry->s.st_mode) && opts.show_directories) ||
            (S_ISLNK(entry->s.st_mode) && opts.show_symlinks) ||
            (S_ISREG(entry->s.st_mode) && opts.show_files))
        {
            if ((S_ISDIR(entry->s.st_mode) && opts.show_tree) &&
                (strcmp(entry->d->d_name, ".") == 0 ||
                 strcmp(entry->d->d_name, "..") == 0))
                continue;

            entries = realloc(entries, (entry_count + 1) *
                                           sizeof(struct laser_dirent *));

            size_t entry_size = sizeof(struct laser_dirent) +
                                offsetof(struct dirent, d_name) +
                                strlen(entry->d->d_name) + 1;

            entries[entry_count] = malloc(entry_size);

            entries[entry_count]->git_status = entry->git_status;

            entries[entry_count]->s = entry->s;
            entries[entry_count]->d = malloc(offsetof(struct dirent, d_name) +
                                             strlen(entry->d->d_name) + 1);

            memcpy(entries[entry_count]->d, entry->d,
                   offsetof(struct dirent, d_name) + strlen(entry->d->d_name) +
                       1);

            entry_count++;
        }

        char *ownername = getpwuid(entry->s.st_uid)->pw_name;
        ssize_t ownername_len = strlen(ownername);
        if (ownername_len > longest_ownername)
            longest_ownername = ownername_len;
    }
    free(entry); // entry is no longer needed it's been copied to entries
                 // u see my dumbass created mem leaks

    lua_getglobal(L, "L_pre_print_entries");
    if (lua_pcall(L, 0, 0, 0) != LUA_OK)
    {
        fprintf(stderr, "lsr: lua error (L_pre_print_entries): %s\n",
                lua_tostring(L, -1));
        lua_pop(L, 1); // pop error message
        exit(1);
    }

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

    laser_process_entries(opts, depth, max_depth, indent);
    free(indent);
}
