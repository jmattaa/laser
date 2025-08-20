#include "laser.h"
#include "colors.h"
#include "filetypes/checktypes.h"
#include "git/lgit.h"
#include "init_lua.h"
#include "laser_pwuid.h"
#include "logger.h"
#include "lua_filters.h"
#include "utils.h"
#include <fcntl.h>
#include <lua.h>
#include <pwd.h>
#include <sys/stat.h>

#define BLOCK_SIZE 512
#define BRANCH_SIZE 8

#define INITIAL_ENTRIES_CAPACITY 16
#define ENTRIES_GROWTH_FACTOR 2

static ssize_t longest_ownername = 0;
static size_t current_dir_total_size = 0;

// ------------------------- helper function decl -----------------------------
static void laser_process_entries(laser_opts opts, int depth, char *indent);
static void laser_handle_entry(struct laser_dirent *entry,
                               const char *full_path, char *indent, int depth,
                               laser_opts opts, int is_last);
static int laser_cmp_dirent(const void *a, const void *b, const void *arg);
static void laser_print_long_entry(struct laser_dirent *entry,
                                   const char *color, char *indent,
                                   const char *branch, laser_opts opts);
static void laser_print_entry(struct laser_dirent *entry, const char *color,
                              char *indent, int depth, laser_opts opts,
                              int is_last);
static laser_color_type laser_color_for_format(const char *filename);

// returns size of directory if able. else returns -1
static off_t laser_get_dir_size(struct laser_dirent *entry, char *fp);
static void laser_list_directory(laser_opts opts, int depth);
// ----------------------------------------------------------------------------

void laser_start(laser_opts opts)
{
    laser_list_directory(opts, 0);

    if (opts.show_directory_size)
    {
        const char *errtemp =
            "error when trying to call LASER_BUILTIN_formatSize: %s\n"
            "Please open a issue on github with this message as this is"
            " an internal error!\n";

        lua_getglobal(L, "LASER_BUILTIN_formatSize");
        lua_pushinteger(L, current_dir_total_size);

        if (lua_pcall(L, 1, 1, 0) != LUA_OK)
        {
            laser_logger_error(errtemp, lua_tostring(L, -1));
            lua_pop(L, 1);
            return;
        }

        const char *size = lua_tostring(L, -1);
        if (size == NULL)
        {
            laser_logger_error(errtemp, "");
            lua_pop(L, 1);
            return;
        }
        printf("\nTotal size: %s\n", size);
    }
}

void laser_process_single_file(laser_opts opts)
{
    struct stat file_stat;
    if (lstat(opts.dir, &file_stat) == -1)
    {
        laser_logger_error("Couldn't stat file %s, %s\n", opts.dir,
                           strerror(errno));
        return;
    }

    struct laser_dirent entry;
    entry.s = file_stat;
    entry.d = malloc(sizeof(struct dirent) + strlen(opts.dir) + 1);
    if (entry.d == NULL)
        laser_logger_fatal(1, "Failed to allocate entry struct: %s",
                           strerror(errno));

    strcpy(entry.d->d_name, opts.dir);

    // default status to ' '
    entry.git_status = ' ';
    if (opts.show_git->show_git_status)
        lgit_getGitStatus(opts, &entry, opts.dir);

    char *ownername = laser_getpwuid(entry.s.st_uid)->name;
    longest_ownername = strlen(ownername); // this has to be the longest name
                                           // cus it be the ownly name

    laser_handle_entry(&entry, opts.dir, "", 0, opts, 1);

    free(entry.d);
}

// ------------------------- helper function impl -----------------------------
static void laser_list_directory(laser_opts opts, int depth)
{
    if (opts.recursive_depth >= 0 && depth > opts.recursive_depth)
        return;

    const char *pipe = "│   ";
    size_t indent_len = depth > 0 ? depth * strlen(pipe) : 0;
    char *indent = malloc(indent_len + 1);
    if (indent == NULL)
        laser_logger_fatal(1, "Malloc function failed: %s", strerror(errno));

    indent[0] = '\0';
    if (depth > 0)
        for (int i = 1; i < depth; i++)
            strcat(indent, pipe);

    laser_process_entries(opts, depth, indent);
    free(indent);
}

static void laser_process_entries(laser_opts opts, int depth, char *indent)
{
    DIR *dir = opendir(opts.dir);
    if (dir == NULL)
    {
        laser_logger_error("couldn't open %s, %s\n", opts.dir, strerror(errno));
        return;
    }

    struct laser_dirent *entry = malloc(sizeof(struct laser_dirent));
    if (entry == NULL)
        laser_logger_fatal(1, "Failed to allocate entry struct: %s",
                           strerror(errno));

    struct laser_dirent **entries = NULL;
    size_t entries_capacity = 0;
    if (opts.sort)
    {
        entries_capacity = INITIAL_ENTRIES_CAPACITY;
        entries = malloc(sizeof(*entries) * entries_capacity);
        if (entries == NULL)
            laser_logger_fatal(1, "Failed to allocate entries struct: %s",
                               strerror(errno));
    }

    lua_getglobal(L, "L_pre_print_entries");
    if (lua_pcall(L, 0, 0, 0) != LUA_OK)
    {
        laser_logger_error("lua error (L_pre_print_entries): %s\n",
                           lua_tostring(L, -1));
        lua_pop(L, 1); // pop error message
        exit(1);
    }

    size_t entry_count = 0;
    int entry_ignored = 0;
    char full_path[LASER_PATH_MAX];
    while ((entry->d = readdir(dir)) != NULL)
    {
        snprintf(full_path, sizeof(full_path), "%s/%s", opts.dir,
                 entry->d->d_name);

        if (lstat(full_path, &entry->s) == -1)
        {
            laser_logger_error("%s\n", strerror(errno));
            continue;
        }

        if (opts.show_git->hide_git_ignored)
        {
            if (strcmp(entry->d->d_name, ".git") == 0)
                continue;

            if (git_ignore_path_is_ignored(&entry_ignored, opts.git_repo,
                                           strncmp(full_path, "./", 2) == 0
                                               ? &full_path[2]
                                               : full_path) < 0)
            {
                laser_logger_error("%s\n", git_error_last()->message);
                continue;
            }

            if (entry_ignored == 1)
                continue;
        }

        entry->git_status = ' ';
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
            if ((S_ISDIR(entry->s.st_mode) &&
                 (opts.show_recursive || opts.show_directory_size)) &&
                (strcmp(entry->d->d_name, ".") == 0 ||
                 strcmp(entry->d->d_name, "..") == 0))
                continue;

            if (opts.show_directory_size)
            {
                if (S_ISDIR(entry->s.st_mode))
                {
                    off_t dirsize = laser_get_dir_size(entry, full_path);
                    if (dirsize == -1)
                        laser_logger_error(
                            "couldn't calculate the size of %s\n",
                            entry->d->d_name);
                    else
                        entry->s.st_size = dirsize;
                }
                current_dir_total_size += entry->s.st_size;
            }

            char *ownername = laser_getpwuid(entry->s.st_uid)->name;
            ssize_t ownername_len = strlen(ownername);
            if (ownername_len > longest_ownername)
                longest_ownername = ownername_len;

            if (!opts.sort)
            {
                laser_handle_entry(entry, full_path, indent, depth, opts, 0);
                continue;
            }

            if (entry_count >= entries_capacity)
            {
                entries_capacity += ENTRIES_GROWTH_FACTOR;
                entries = realloc(entries, sizeof(*entries) * entries_capacity);
                if (entries == NULL)
                    laser_logger_fatal(1, "Failed to realloc entries: %s",
                                       strerror(errno));
            }

            size_t entry_size = sizeof(struct laser_dirent) +
                                offsetof(struct dirent, d_name) +
                                strlen(entry->d->d_name) + 1;

            entries[entry_count] = malloc(entry_size);
            if (entries[entry_count] == NULL)
                laser_logger_fatal(1, "Failed to allocate entry struct: %s",
                                   strerror(errno));

            entries[entry_count]->git_status = entry->git_status;
            entries[entry_count]->s = entry->s;
            entries[entry_count]->d = malloc(offsetof(struct dirent, d_name) +
                                             strlen(entry->d->d_name) + 1);
            if (entries[entry_count]->d == NULL)
                laser_logger_fatal(1, "Failed to allocate dirent struct: %s",
                                   strerror(errno));

            memcpy(entries[entry_count]->d, entry->d,
                   offsetof(struct dirent, d_name) + strlen(entry->d->d_name) +
                       1);

            entry_count++;
        }
    }

    free(entry->d);
    free(entry); // entry is no longer needed it's been copied to entries
                 // u see my dumbass created mem leaks

    if (!opts.sort)
        goto cleanup;

    // sort and print stuff
    laser_sort(entries, entry_count, sizeof(struct laser_dirent *),
               laser_cmp_dirent, NULL);

    for (size_t i = 0; i < entry_count; i++)
    {
        size_t is_last = (i == entry_count - 1);

        snprintf(full_path, sizeof(full_path), "%s/%s", opts.dir,
                 entries[i]->d->d_name);

        laser_handle_entry(entries[i], full_path, indent, depth, opts, is_last);

        free(entries[i]->d);
        free(entries[i]);
    }

cleanup:
    if (entries != NULL)
        free(entries);
    closedir(dir);
}

static void laser_handle_entry(struct laser_dirent *entry,
                               const char *full_path, char *indent, int depth,
                               laser_opts opts, int is_last)
{
    if (S_ISDIR(entry->s.st_mode))
    {
        laser_print_entry(entry, LASER_COLORS[LASER_COLOR_DIR].value, indent,
                          depth, opts, is_last);
        if (opts.show_recursive)
        {
            laser_opts sub_opts = opts;
            sub_opts.dir = full_path;
            laser_list_directory(sub_opts, depth + 1);
        }

        return;
    }

    if (S_ISLNK(entry->s.st_mode) && opts.show_symlinks)
    {
        char symlink_target[LASER_PATH_MAX];
        int len =
            readlink(full_path, symlink_target, sizeof(symlink_target) - 1);
        if (len != -1)
        {
            symlink_target[len] = '\0';

            char res_string[LASER_PATH_MAX * 2 + 4]; // 4 is " -> "

            snprintf(res_string, sizeof(res_string), "%s -> %s",
                     entry->d->d_name, symlink_target);

            struct laser_dirent *ent = malloc(sizeof(struct laser_dirent));
            if (ent == NULL)
                laser_logger_fatal(1, "Failed to allocate entry struct: %s",
                                   strerror(errno));

            ent->s = entry->s;
            ent->d = malloc(sizeof(struct dirent));
            if (ent->d == NULL)
                laser_logger_fatal(1, "Failed to allocate entry struct: %s",
                                   strerror(errno));

            strcpy(ent->d->d_name, res_string);

            ent->git_status = entry->git_status;

            laser_print_entry(ent, LASER_COLORS[LASER_COLOR_SYMLINK].value,
                              indent, depth, opts, is_last);

            free(ent->d);
            free(ent);
        }
        return;
    }

    if (laser_is_filestat_exec(&entry->s))
        laser_print_entry(entry, LASER_COLORS[LASER_COLOR_EXEC].value, indent,
                          depth, opts, is_last);
    else if (entry->d->d_name[0] == '.')
        laser_print_entry(entry, LASER_COLORS[LASER_COLOR_HIDDEN].value, indent,
                          depth, opts, is_last);
    else
    {
        // coloring which depends on formats
        laser_color_type color_type = laser_color_for_format(full_path);
        if (color_type != LASER_COLOR_FILE)
            laser_print_entry(entry, LASER_COLORS[color_type].value, indent,
                              depth, opts, is_last);
        else if (S_ISREG(entry->s.st_mode))
            laser_print_entry(entry, LASER_COLORS[LASER_COLOR_FILE].value,
                              indent, depth, opts, is_last);
    }
}

// last parameter is only to match the signature for laser_sort
static int laser_cmp_dirent(const void *a, const void *b, const void *_)
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
        laser_logger_error("error in L_compare_entries: %s\n",
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
                                   const char *branch, laser_opts opts)
{
    lua_getglobal(L, "L_long_format");

    lua_newtable(L);

    lua_pushstring(L, entry->d->d_name);
    lua_setfield(L, -2, "name");

    lua_pushinteger(L, entry->s.st_mode);
    lua_setfield(L, -2, "mode");

    off_t size = entry->s.st_size;
    if (S_ISDIR(entry->s.st_mode) && !opts.show_directory_size)
        size = -1;
    lua_pushinteger(L, size);
    lua_setfield(L, -2, "size");

    lua_pushinteger(L, entry->s.st_mtime);
    lua_setfield(L, -2, "mtime");

    lua_pushstring(L, laser_getpwuid(entry->s.st_uid)->name);
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
        laser_logger_error("error in long_format: %s\n", lua_tostring(L, -1));
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

static void laser_print_entry(struct laser_dirent *entry, const char *color,
                              char *indent, int depth, laser_opts opts,
                              int is_last)
{
    char branch[BRANCH_SIZE] = "";
    if (depth > 0)
        strcpy(branch, is_last ? "└─ " : "├─ ");

    if (opts.show_long)
    {
        laser_print_long_entry(entry, color, indent, branch, opts);
        return;
    }

    printf("%s%s%s%s%s", indent, branch, color, entry->d->d_name,
           LASER_COLORS[LASER_COLOR_RESET].value);
    (entry->git_status && entry->git_status != ' ')
        ? printf("\x1b[33m [%c]\x1b[0m", entry->git_status)
        : 0;
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

static off_t laser_get_dir_size(struct laser_dirent *ent, char *fp)
{
    if (!S_ISDIR(ent->s.st_mode))
        return -1;

    DIR *dir = opendir(fp);
    if (dir == NULL)
    {
        laser_logger_error("couldn't open %s, %s\n", ent->d->d_name,
                           strerror(errno));
        return -1;
    }

    off_t s = 0;

    struct laser_dirent e;
    size_t fp_len = strlen(fp);

    char full_path[LASER_PATH_MAX];
    memcpy(full_path, fp, fp_len);
    full_path[fp_len] = '/';

    while ((e.d = readdir(dir)) != NULL)
    {
        if (strcmp(e.d->d_name, ".") == 0 || strcmp(e.d->d_name, "..") == 0)
            continue;

        // first +1 is to ensure that / is added
        memcpy(full_path + fp_len + 1, e.d->d_name, strlen(e.d->d_name) + 1);
        if (stat(full_path, &e.s) == -1)
        {
            // just ignore Eror NO ENTry
            if (errno == ENOENT)
                continue;

            laser_logger_error("couldn't stat %s, %s\n", full_path,
                               strerror(errno));
            continue;
        }

        if (S_ISDIR(e.s.st_mode))
        {
            off_t sub_s = laser_get_dir_size(&e, full_path);
            if (sub_s == -1) // unable to calculate the size
            {
                closedir(dir);
                return -1;
            }
            s += sub_s;
        }
        else
            // s += e.s.st_size; // this will give the logical size
            // while this will give how much space is on disk
            s += e.s.st_blocks * BLOCK_SIZE;
    }

    closedir(dir);

    return s;
}
// ----------------------------------------------------------------------------
