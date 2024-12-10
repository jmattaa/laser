#include "cli.h"
#include "init_lua.h"

static const struct option long_args[] = {
    {"all", 0, 0, 'a'},
    {"Files", 0, 0, 'F'},
    {"Directories", 0, 0, 'D'},
    {"Symlinks", 0, 0, 'S'},
    {"Git", 0, 0, 'G'},
    {"long", 0, 0, 'l'},
    {"recursive", optional_argument, 0, 'r'},
    {"filter", required_argument, 0, 'f'},
    {"version", 0, 0, 'v'},
    {"help", 0, 0, 'h'},
    {"completions", required_argument, 0, 0},
    {0, 0, 0, 0}};

// UPDATE THIS TO MATCH long_args!!
static const char *descriptions[] = {
    "Show all entries, including hidden",
    "Show files only",
    "Show directories only",
    "Show symlinks only",
    "Show entries that are not defined in .gitignore",
    "Use long format",
    "Show in tree format",
    "Filter out files using lua filters (`L_filters` in lsr.lua)",
    "Print the current version",
    "Print help message",
    "Generate shell completions",
};

#define L_DEFAULT_SIMPLE_ARGS(_X)                                              \
    _X(all, boolean)                                                           \
    _X(files, boolean)                                                         \
    _X(directories, boolean)                                                   \
    _X(symlinks, boolean)                                                      \
    _X(git, boolean)                                                           \
    _X(long, boolean)

#define L_DEFAULT_SIMPLE_ARGS_GET(arg, type)                                   \
    lua_pushstring(L, #arg);                                                   \
    lua_gettable(L, -2);                                                       \
    if (lua_is##type(L, -1))                                                   \
        *show_##arg = lua_toboolean(L, -1);                                    \
    lua_pop(L, 1);

static void lua_get_L_default_args(int *show_all, int *show_files,
                                   int *show_directories, int *show_symlinks,
                                   int *show_git, int *show_long,
                                   const char ***filters, int *filter_count)
{
    lua_getglobal(L, "L_default_args");
    if (!lua_istable(L, -1))
        return;

    L_DEFAULT_SIMPLE_ARGS(L_DEFAULT_SIMPLE_ARGS_GET)

    lua_pushstring(L, "filters");
    lua_gettable(L, -2);
    if (lua_istable(L, -1))
    {
        lua_pushnil(L);
        while (lua_next(L, -2) != 0)
        {
            (*filters) =
                realloc(*filters, sizeof(char *) * ((*filter_count) + 1));
            (*filters)[*filter_count] = strdup(lua_tostring(L, -1));
            (*filter_count)++;
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
}

#define L_DEFAULT_ARG_TYPES(_X)                                                \
    _X(show_files)                                                             \
    _X(show_directories)                                                       \
    _X(show_symlinks)

laser_opts laser_cli_parsecmd(int argc, char **argv)
{
    int show_all = 0;
    int show_files = -1;
    int show_directories = -1;
    int show_symlinks = -1;
    int recursive_depth = -1;
    int show_git = 0;
    int show_tree = 0;
    int show_long = 0;
    const char *dir = ".";

    int filter_count = 0;
    const char **filters = NULL;

    int opt;

#define _X(name) int default_##name;
    L_DEFAULT_ARG_TYPES(_X)
#undef _X

#define _X(name) &name,
    lua_get_L_default_args(&show_all, L_DEFAULT_ARG_TYPES(_X)(&show_git),
                           &show_long, &filters, &filter_count);
#undef _X

    // set default recursive_depth from lua
    lua_getglobal(L, "L_recursive_max_depth");
    if (lua_isnumber(L, -1))
        recursive_depth = (int)lua_tointeger(L, -1);

    while ((opt = getopt_long(argc, argv, "aFDSGr::lvhf::", long_args, NULL)) !=
           -1)
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
                show_git = 1;
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
            case 'v':
                printf("laser %s\n", LASER_VERSION);
                exit(0);
            case 'h':
                laser_cli_print_help();
                exit(0);
            case 'f':
                if (optarg == NULL)
                {
                    fprintf(stderr,
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
                if (strcmp(long_args[optind - 1].name, "completions") == 0)
                {
                    laser_cli_generate_completions(argv[optind - 1]);
                    exit(0);
                }
                break;
            default:
                exit(1);
        }
    }

    if (optind < argc)
        dir = argv[optind];

    // check if default lua values have been overriden by cli
    // if not then use them if yes then use cli values
#define _X(name) name == -1 &&
    if (L_DEFAULT_ARG_TYPES(_X) 1) // there is prolly a better way than to put 1
#undef _X
    {
#define _X(name) name = default_##name;
        L_DEFAULT_ARG_TYPES(_X)
#undef _X
    }

    return (laser_opts){
        show_all,  show_files,      show_directories, show_symlinks, show_git,
        show_tree, show_long,       recursive_depth,  filter_count,  filters,
        dir,       .parentDir = dir};
}

void laser_cli_generate_completions(const char *shell)
{
    if (strcmp(shell, "bash") == 0)
    {
        printf("_lsr() {\n");
        printf("    local cur prev opts\n");
        printf("    COMPREPLY=()\n");
        printf("    cur=\"${COMP_WORDS[COMP_CWORD]}\"\n");

        printf("    opts=\"");
        for (int i = 0; long_args[i].name != NULL; i++)
        {
            char short_flag = long_args[i].val ? long_args[i].val : '\0';
            if (short_flag)
            {
                printf("-%c ", short_flag);
            }
            printf("--%s ", long_args[i].name);
        }
        printf("\"\n");

        printf("    if [[ ${cur} == -* ]]; then\n");
        printf("        COMPREPLY=( $(compgen -W \"${opts}\" -- ${cur}) )\n");
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
            char short_flag = long_args[i].val ? long_args[i].val : '\0';
            if (short_flag)
            {
                printf("        '(-%c --%s)'{-%c,--%s}'[%s]' \\\n", short_flag,
                       long_args[i].name, short_flag, long_args[i].name,
                       descriptions[i]);
            }
            else
            {
                printf("        '--%s[%s]' \\\n", long_args[i].name,
                       descriptions[i]);
            }
        }
        printf("        '*:file:_files'\n");
        printf("}\n");

        printf("compdef _lsr lsr\n");
    }
    else if (strcmp(shell, "fish") == 0)
    {
        printf("# Fish completions for lsr\n");
        for (int i = 0; long_args[i].name != NULL; i++)
        {
            printf("complete -c lsr");
            if (long_args[i].val)
            {
                printf(" -s %c", long_args[i].val);
            }
            printf(" -l %s -d '%s'\n", long_args[i].name, descriptions[i]);
        }
    }
    else
    {
        fprintf(stderr,
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
        char short_flag = long_args[i].val ? long_args[i].val : '\0';
        if (short_flag)
            printf("  -%c, --%-28s %s\n", short_flag, long_args[i].name,
                   descriptions[i]);
        else
            printf("      --%-28s %s\n", long_args[i].name, descriptions[i]);
    }
}

void laser_cli_destroy_opts(laser_opts opts)
{
    for (int i = 0; i < opts.filter_count; i++)
        free((void *)opts.filters[i]);
    free(opts.filters);
}
