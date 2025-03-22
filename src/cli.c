#include "cli.h"
#include "init_lua.h"
#include "logger.h"
#include <errno.h>
#include <git2.h>
#include <git2/repository.h>

static int completionsset;

#define ARGS_ITER(_X, ...)                                                     \
    _X("all", 0, 0, 'a', "Show all entries, including hidden")                 \
    _X("Files", 0, 0, 'F', "Show files only")                                  \
    _X("Directories", 0, 0, 'D', "Show directories only")                      \
    _X("Symlinks", 0, 0, 'S', "Show symlinks only")                            \
    _X("Git", optional_argument, 0, 'G', "Do not show ignored git files")      \
    _X("git-status", optional_argument, 0, 'g', "Show git status for entries") \
    _X("git-ignored", optional_argument, 0, 'i', "Ignore git ignored files")   \
    _X("long", 0, 0, 'l', "Use long format")                                   \
    _X("recursive", optional_argument, 0, 'r', "Show in tree format")          \
    _X("filter", required_argument, 0, 'f',                                    \
       "Filter out files using lua filters (L_filters in lsr.lua)")            \
    _X("ensure-colors", 0, 0, 'c', "Force colored output")                     \
    _X("no-lua", 0, 0, '!',                                                    \
       "Don't use user defined configuration from lsr.lua")                    \
    _X("version", 0, 0, 'v', "Print the current version")                      \
    _X("help", 0, 0, 'h', "Print help message")                                \
    _X("completions", required_argument, &completionsset, 1,                   \
       "Generate shell completions")

#define _X(name, a, b, short, ...) {name, a, b, short},
static const struct option long_args[] = {ARGS_ITER(_X){0, 0, 0, 0}};
#undef _X
#define _X(name, a, b, short, description) description,
static const char *descriptions[] = {ARGS_ITER(_X)};
#undef _X

#define L_DEFAULT_SIMPLE_ARGS(_X)                                              \
    _X(all, 0, show_)                                                          \
    _X(files, -1, default_show_)                                               \
    _X(directories, -1, default_show_)                                         \
    _X(symlinks, -1, default_show_)                                            \
    _X(long, 0, show_)                                                         \
    _X(ensure_colors, 0, show_)

#define L_ADVANCED_DEFAULT_ARGS_IN_SIMPLE_ARGS(_X)                             \
    _X(show_files)                                                             \
    _X(show_directories)                                                       \
    _X(show_symlinks)

#define _X(name, ...) int *show_##name,
static void
lua_get_L_default_args(L_DEFAULT_SIMPLE_ARGS(_X) struct lgit_show_git *show_git,
                       const char ***filters, int *filter_count);
#undef _X

laser_opts laser_cli_parsecmd(int argc, char **argv)
{
    int recursive_depth = -1;

    struct lgit_show_git *show_git = malloc(sizeof(struct lgit_show_git));
    if (show_git == NULL)
        laser_logger_fatal(1, "Failed to allocate show_git struct: %s",
                           strerror(errno));

    show_git->show_git_status = 0;
    show_git->hide_git_ignored = 0;

    git_repository *git_repo = NULL;
    int show_tree = 0;

    const char *dir = ".";
    const char *gitDir = dir;

    int filter_count = 0;
    const char **filters = NULL;

    int opt;

#define _X(name, default, ...) int show_##name = default;
    L_DEFAULT_SIMPLE_ARGS(_X)
#undef _X

#define _X(name) int default_##name = -1;
    L_ADVANCED_DEFAULT_ARGS_IN_SIMPLE_ARGS(_X)
#undef _X

#define _X(name, default, prefix) &prefix##name,
    lua_get_L_default_args(L_DEFAULT_SIMPLE_ARGS(_X) show_git, &filters,
                           &filter_count);
#undef _X

    // set default recursive_depth from lua
    lua_getglobal(L, "L_recursive_max_depth");
    if (lua_isnumber(L, -1))
        recursive_depth = (int)lua_tointeger(L, -1);

    while ((opt = getopt_long(argc, argv, "aFDSG::g::i::r::lcvhf::!", long_args,
                              NULL)) != -1)
    {
        switch (opt)
        {
            case 'a':
                show_all = 1;
                break;
            case 'F':
                show_directories = show_directories <= 0 ? 0 : 1;
                show_symlinks = show_symlinks <= 0 ? 0 : 1;
                show_files = 1;
                break;
            case 'D':
                show_symlinks = show_symlinks <= 0 ? 0 : 1;
                show_files = show_files <= 0 ? 0 : 1;
                show_directories = 1;
                break;
            case 'S':
                show_files = show_files <= 0 ? 0 : 1;
                show_directories = show_directories <= 0 ? 0 : 1;
                show_symlinks = 1;
                break;
            case 'G':
                show_git->show_git_status = 1;
                show_git->hide_git_ignored = 1;
                if (optarg != NULL)
                    gitDir = optarg;
                break;
            case 'g':
                show_git->show_git_status = 1;
                if (optarg != NULL)
                    gitDir = optarg;
                break;
            case 'i':
                show_git->hide_git_ignored = 1;
                if (optarg != NULL)
                    gitDir = optarg;
                break;
            case 'r':
                show_tree = 1;
                // recursive listing has to ovveride dir flag
                show_directories = 1;

                if (optarg == NULL)
                    break;

                char *end;
                recursive_depth = strtol(optarg, &end, 10);
                if (end == optarg)
                    break;

                break;
            case 'l':
                show_long = 1;
                break;
            case 'c':
                show_ensure_colors = 1;
                break;
            case 'v':
                printf("laser %s\n", LASER_VERSION);
                exit(0);
            case 'h':
                laser_cli_print_help();
                exit(0);
            case 'f':
                if (optarg == NULL)
                {
                    laser_logger_error(
                        "lsr: couldn't filter files, filter not passed or "
                        "not formatted correctly\nlsr: filter usage "
                        "`-fmyfilter` or `--filter=myfilter\n");
                    exit(1);
                }

                filters = realloc(filters, sizeof(char *) * (filter_count + 1));
                filters[filter_count] = strdup(optarg);
                filter_count++;

                break;
            case 0:
                if (completionsset)
                {
                    laser_cli_generate_completions(optarg);
                    exit(0);
                }
                break;
            case '!':
                break;
            default:
                exit(1);
        }
    }

    if (optind < argc)
        dir = argv[optind];

    // check if default lua values have been overriden by cli
    // if not then use them if yes then use cli values
    // this is for the stuff with -1 vals
    // symlinks, files, directories
#define _X(name, ...) name == -1 &&
    if (L_ADVANCED_DEFAULT_ARGS_IN_SIMPLE_ARGS(
            _X) 1) // there is prolly a better way than to put 1
#undef _X
    {
#define _X(name, ...) name = default_##name;
        L_ADVANCED_DEFAULT_ARGS_IN_SIMPLE_ARGS(_X)
#undef _X
    }

    if (show_git->show_git_status || show_git->hide_git_ignored)
    {
        int err = git_repository_open(&git_repo, gitDir);
        if (err != 0)
        {
            laser_logger_error("couldn't open git repo at '%s'\n", gitDir);
            show_git = 0;
        }
    }

    return (laser_opts){show_all,      show_files,      show_directories,
                        show_symlinks, show_git,        git_repo,
                        show_tree,     show_long,       recursive_depth,
                        filter_count,  filters,         show_ensure_colors,
                        dir,           .parentDir = dir};
}

void laser_cli_generate_completions(const char *shell)
{
    if (strcmp(shell, "bash") == 0)
    {
        printf("_lsr() {\n");
        printf("    local cur prev opts opts_with_args\n");
        printf("    COMPREPLY=()\n");
        printf("    cur=\"${COMP_WORDS[COMP_CWORD]}\"\n");

        printf("    opts=\"");
        for (int i = 0; long_args[i].name != NULL; i++)
        {
            if (long_args[i].val != 1)
            {
                printf("-%c ", long_args[i].val);
            }
            printf("--%s ", long_args[i].name);
        }
        printf("\"\n");

        printf("    opts_with_args=\"");
        for (int i = 0; long_args[i].name != NULL; i++)
        {
            if (long_args[i].has_arg)
            {
                printf("--%s ", long_args[i].name);
            }
        }
        printf("\"\n");

        printf("    if [[ ${cur} == --* ]]; then\n");
        printf("        COMPREPLY=( $(compgen -W \"${opts}\" -- ${cur}) )\n");
        printf("    elif [[ ${opts_with_args} =~ ${prev} ]]; then\n");
        printf("        compopt -o nospace\n");
        printf("    fi\n");

        printf("    return 0\n");
        printf("}\n");
        printf("complete -F _lsr lsr\n");
    }
    else if (strcmp(shell, "zsh") == 0)
    {
        printf("#compdef lsr\n");
        printf("autoload -U is-at-least\n");
        printf("_lsr() {\n");
        printf("    _arguments -s \\\n");
        for (int i = 0; long_args[i].name != NULL; i++)
        {
            if (long_args[i].has_arg)
            {
                printf("        \"--%s[Specify %s]:%s:\" \\\n",
                       long_args[i].name, long_args[i].name, descriptions[i]);
            }
            else if (long_args[i].val != 1)
            {
                printf("        \"(-%c --%s)\"{-%c,--%s}\"[%s]\" \\\n",
                       long_args[i].val, long_args[i].name, long_args[i].val,
                       long_args[i].name, descriptions[i]);
            }
            else
            {
                printf("        \"--%s[%s]\" \\\n", long_args[i].name,
                       descriptions[i]);
            }
        }
        printf("        \"*:file:_files\"\n");
        printf("}\n");

        printf("compdef _lsr lsr\n");
    }
    else if (strcmp(shell, "fish") == 0)
    {
        printf("# Fish completions for lsr\n");
        for (int i = 0; long_args[i].name != NULL; i++)
        {
            printf("complete -c lsr");
            if (long_args[i].val != 1)
            {
                printf(" -s %c", long_args[i].val);
            }
            printf(" -l %s -d \"%s\"", long_args[i].name, descriptions[i]);
            if (long_args[i].has_arg)
                printf(" -r");
            printf("\n");
        }
    }
    else
    {
        laser_logger_error(
            "lsr: unsupported shell '%s'. Supported shells are bash, "
            "zsh, and fish.\n",
            shell);
        exit(1);
    }
}

void laser_cli_print_help(void)
{
    printf("Usage: lsr [options] [directory]\n");
    printf("Options:\n");
    for (int i = 0; long_args[i].name != NULL; i++)
    {
        if (long_args[i].val != 1)
            printf("  -%c, --%-28s %s\n", long_args[i].val, long_args[i].name,
                   descriptions[i]);
        else
            printf("      --%-28s %s\n", long_args[i].name, descriptions[i]);
    }
}

void laser_cli_destroy_opts(laser_opts opts)
{
    if (opts.git_repo)
        git_repository_free(opts.git_repo);

    free(opts.show_git);

    for (int i = 0; i < opts.filter_count; i++)
        free((void *)opts.filters[i]);
    free(opts.filters);
}

// ------------------------- helper function decl -----------------------------
#define L_DEFAULT_SIMPLE_ARGS_GET(arg, ...)                                    \
    lua_pushstring(L, #arg);                                                   \
    lua_gettable(L, -2);                                                       \
    if (lua_isboolean(L, -1))                                                  \
        *show_##arg = lua_toboolean(L, -1);                                    \
    lua_pop(L, 1);

#define _X(name, ...) int *show_##name,
static void
lua_get_L_default_args(L_DEFAULT_SIMPLE_ARGS(_X) struct lgit_show_git *show_git,
                       const char ***filters, int *filter_count)
{
#undef _X
    lua_getglobal(L, "L_default_args");
    if (!lua_istable(L, -1))
        return;

    L_DEFAULT_SIMPLE_ARGS(L_DEFAULT_SIMPLE_ARGS_GET);

    lua_pushstring(L, "git");
    lua_gettable(L, -2);
    if (lua_istable(L, -1))
    {
        lua_pushstring(L, "status");
        lua_gettable(L, -2);
        if (lua_isboolean(L, -1))
            show_git->show_git_status = lua_toboolean(L, -1);
        lua_pop(L, 1);
        lua_pushstring(L, "ignored");
        lua_gettable(L, -2);
        if (lua_isboolean(L, -1))
            show_git->hide_git_ignored = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    lua_pushstring(L, "filters");
    lua_gettable(L, -2);
    if (lua_istable(L, -1))
    {
        lua_pushnil(L);
        while (lua_next(L, -2) != 0)
        {
            (*filters) =
                realloc(*filters, sizeof(char *) * ((*filter_count) + 1));
            if (*filters == NULL)
                laser_logger_fatal(
                    1, "Failed to allocate memory for filters, %s\n",
                    strerror(errno));
            (*filters)[*filter_count] = strdup(lua_tostring(L, -1));
            (*filter_count)++;
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
}
