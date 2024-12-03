#ifndef LASER_CLI_H
#define LASER_CLI_H

#include "utils.h"
#include <stdio.h>

#define LASER_VERSION "1.2.0"

typedef struct laser_opts
{
    int show_all;
    int show_files;
    int show_directories;
    int show_symlinks;
    int show_git;
    int show_tree;
    int show_long;
    int recursive_depth;

    int filter_count;
    const char **filters;

    const char *dir;
    const char *parentDir;
} laser_opts;

laser_opts laser_cli_parsecmd(int argc, char **argv);
void laser_cli_generate_completions(const char *shell);
void laser_cli_print_help(void);

void laser_cli_destroy_opts(laser_opts opts);

#endif
