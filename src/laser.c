#include "include/laser.h"
#include "git/include/lgit.h"
#include "include/colors.h"
#include "include/utils.h"

static int laser_max_depth = -1;

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

laser_dir_entries laser_getdirs(laser_opts opts)
{
    DIR *dir_stuff = opendir(opts.dir); // idk what to name this

    laser_dir_entries entries = {0};

    if (dir_stuff == NULL)
    {
        fprintf(stderr, "lsr: couldn't open %s, %s\n", opts.dir,
                strerror(errno));
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

    int gitignore_count = 0;
    char **gitignore_patterns = NULL;

    if (opts.show_git)
    {
        gitignore_patterns =
            lgit_parseGitignore(opts.parentDir, &gitignore_count);
        qsort(gitignore_patterns, gitignore_count, sizeof(char *),
              laser_cmp_string);
    }

    while ((entry = readdir(dir_stuff)) != NULL)
    {
        if (!opts.show_all && entry->d_name[0] == '.')
            continue;

        snprintf(full_path, sizeof(full_path), "%s/%s", opts.dir,
                 entry->d_name);

        if (lstat(full_path, &file_stat) == -1)
        {
            fprintf(stderr, "lsr: %s\n", strerror(errno));
            continue;
        }

        if (opts.show_git && laser_string_in_sorted_array(
                                 strip_parent_dir(full_path, opts.parentDir),
                                 gitignore_patterns, gitignore_count))
            continue;

        if (opts.show_git && strcmp(entry->d_name, ".git") == 0)
            continue;

        if (S_ISDIR(file_stat.st_mode) && opts.show_directories)
        {
            if ((strcmp(entry->d_name, ".") == 0 ||
                 strcmp(entry->d_name, "..") == 0) &&
                opts.show_tree)
                continue;

            if (entries.dir_count == 0)
                entries.dirs = malloc(dir_alloc * sizeof(laser_dir *));

            entries.dirs = laser_grow_dirarray(entries.dirs, &dir_alloc,
                                               entries.dir_count);
            entries.dirs[entries.dir_count] = laser_init_dir(entry->d_name);

            if (opts.show_tree)
            {
                laser_opts sub_opts = opts;
                sub_opts.dir = full_path;

                laser_dir_entries subentries = laser_getdirs(sub_opts);
                entries.dirs[entries.dir_count]->sub_entries = subentries;
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
                if (entries.symlink_count == 0)
                    free(entries.symlinks);
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

    if (gitignore_patterns)
    {
        for (int i = 0; i < gitignore_count; i++)
        {
            free(gitignore_patterns[i]);
        }
        free(gitignore_patterns);
    }

    laser_max_depth++;

    return entries;
}

int laser_cmp_dir(const void *a, const void *b)
{
    // my brother chatgpt told me to cast to ** and then to *
    // cuz qsort gives us a pointer and then we cast idk????
    laser_dir *dir_a = *(laser_dir **)a;
    laser_dir *dir_b = *(laser_dir **)b;

    return strcmp(dir_a->name, dir_b->name);
}

void laser_print_entries(int entries_count, char **entries, const char *color,
                         char *indent, int depth)
{
    if (entries_count <= 0)
        return;

    for (int i = 0; i < entries_count; i++)
    {
        char branch[depth > 0 ? 8 : 1]; // 8 cuz the ting be 8 chars long
        if (depth > 0)
        {
            if (i == entries_count - 1)
                strcpy(branch, "└─ ");
            else
                strcpy(branch, "├─ ");
        }
        else
            branch[0] = '\0';

        printf("%s%s%s%s" RESET_COLOR "\n", indent, branch, color, entries[i]);

        free(entries[i]);
    }

    free(entries);
}

void laser_list(laser_dir_entries lentries, int depth)
{
    char *pipe = "│   ";
    size_t indent_len = depth > 0 ? depth * strlen(pipe) : 0;
    char *indent = malloc(indent_len);

    indent[0] = '\0';
    if (depth > 0)
        for (int i = 1; i < depth; i++) // so we dont do no pipe in the begining
            strcat(indent, pipe);

    if (lentries.dir_count > 0)
        qsort(lentries.dirs, lentries.dir_count, sizeof(laser_dir *),
              laser_cmp_dir);

    if (lentries.file_count > 0)
        qsort(lentries.files, lentries.file_count, sizeof(char *),
              laser_cmp_string);

    if (lentries.hidden_count > 0)
        qsort(lentries.hidden, lentries.hidden_count, sizeof(char *),
              laser_cmp_string);

    if (lentries.symlink_count > 0)
        qsort(lentries.symlinks, lentries.symlink_count, sizeof(char *),
              laser_cmp_string);

    if (lentries.dir_count > 0)
    {
        for (int i = 0; i < lentries.dir_count; i++)
        {
            char branch[8];
            if (depth > 0)
                strcpy(branch, "├─ ");
            else
                branch[0] = '\0';

            printf("%s%s%s%s" RESET_COLOR "\n", indent, branch, DIR_COLOR,
                   lentries.dirs[i]->name);

            laser_list(lentries.dirs[i]->sub_entries, depth + 1);

            laser_free_dir(lentries.dirs[i]);
        }
        free(lentries.dirs);

        if (depth < 1)
            printf("\n");
    }

    laser_print_entries(lentries.file_count, lentries.files, FILE_COLOR, indent,
                        depth);

    laser_print_entries(lentries.hidden_count, lentries.hidden, HIDDEN_COLOR,
                        indent, depth);
    laser_print_entries(lentries.symlink_count, lentries.symlinks,
                        SYMLINK_COLOR, indent, depth);

    if (depth < 1)
        printf("\n");

    free(indent);
}
