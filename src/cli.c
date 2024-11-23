#include "cli.h"

laser_opts laser_cli_parsecmd(int argc, char **argv)
{
    int show_all = 0;
    int show_files = -1;
    int show_directories = -1;
    int recursive_depth = -1;
    int show_symlinks = -1;
    int show_git = 0;
    int show_tree = 0;
    int show_long = 0;
    const char *dir = ".";

    int opt;

    struct option long_args[] = {{"all", 0, 0, 'a'},
                                 {"Files", 0, 0, 'F'},
                                 {"Directories", 0, 0, 'D'},
                                 {"Symlinks", 0, 0, 'S'},
                                 {"Git", 0, 0, 'G'},
                                 {"long", 0, 0, 'l'},
                                 {"recursive", optional_argument, 0, 'r'},
                                 {"completions", required_argument, 0, 0},
                                 {"version", no_argument, 0, 'v'},
                                 {0, 0, 0, 0}};

    while ((opt = getopt_long(argc, argv, "aFDSGr::lv", long_args, NULL)) != -1)
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
                    recursive_depth = -1;

                break;
            case 'l':
                show_long = 1;
                break;
            case 0:
                if (long_args[optind - 1].flag == 0)
                {
                    laser_cli_generate_completions(argv[optind - 1], long_args);
                    exit(0);
                }
            case 'v':
                printf("laser %s\n", LASER_VERSION);
                exit(0);
            default:
                exit(1);
        }
    }

    if (optind < argc)
        dir = argv[optind];

    return (laser_opts){show_all,        show_files,      show_directories,
                        show_symlinks,   show_git,        show_tree,
                        show_long,       recursive_depth, dir,
                        .parentDir = dir};
}

void laser_cli_generate_completions(const char *shell,
                                    struct option long_args[])
{
    // UPDATE THIS TO MATCH long_args!!
    char *descriptions[] = {"Show all entries, including hidden",
                            "Show files only",
                            "Show directories only",
                            "Show symlinks only",
                            "Show entries that are not defined in .gitignore",
                            "Use long format",
                            "Show in tree format",
                            "Generate shell completions"};

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
