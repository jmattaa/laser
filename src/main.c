#include "cli.h"
#include "colors.h"
#include "init_lua.h"
#include "laser.h"
#include "logger.h"
#include <git2.h>
#include <git2/global.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdlib.h>
#include <unistd.h>

// should be set by the compiler
#ifndef DEFAULT_SCRIPT_PATH
#define DEFAULT_SCRIPT_PATH "/usr/local/.lsr"
#endif

int main(int argc, char **argv)
{
    git_libgit2_init();

    if (!laser_init_lua())
        return 1;

    const char *default_script = DEFAULT_SCRIPT_PATH;

    const char *user_script = getenv("HOME");
    char user_config_path[LASER_PATH_MAX];

    if (user_script)
        snprintf(user_config_path, sizeof(user_config_path),
                 "%s/.config/lsr/lsr.lua", user_script);
    else
        user_config_path[0] = '\0';

    const char *script_to_load = default_script;

    unsigned char use_user_config = 1;
    for (int i = 0; i < argc; i++)
        if (strcmp(argv[i], "--no-lua") == 0 || strcmp(argv[i], "-!") == 0)
            use_user_config = 0;

    if (user_config_path[0] != '\0' && access(user_config_path, R_OK) == 0 &&
        use_user_config)
        script_to_load = user_config_path;

    // set the package path of user first, to get priority
    // TODO: THIS IS A VERY HACKY SOLUTION FIND SOMETHING ELSE
    if (script_to_load != default_script)
        laser_lua_set_package_path(script_to_load);
    laser_lua_set_package_path(default_script);

    laser_lua_load_script(default_script); // load the default script cuz user
                                           // may not be defining everything
    if (script_to_load != default_script)
        laser_lua_load_script(script_to_load);

    laser_opts opts = laser_cli_parsecmd(argc, argv);
    laser_colors_init(opts);

    struct stat path_stat;
    if (stat(opts.dir, &path_stat) != 0)
    {
        laser_logger_error("Error checking %s: %s\n", opts.dir,
                           strerror(errno));
        goto clean;
    }

    if (S_ISREG(path_stat.st_mode))
    {
        laser_process_single_file(opts);
        goto clean;
    }
    laser_start(opts);

clean:
    laser_cli_destroy_opts(opts);
    laser_lua_destroy();
    git_libgit2_shutdown();

    return 0;
}
