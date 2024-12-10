#include "cli.h"
#include "colors.h"
#include "init_lua.h"
#include "laser.h"
#include <git2.h>
#include <git2/global.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdlib.h>
#include <unistd.h>

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

    if (user_config_path[0] != '\0' && access(user_config_path, R_OK) == 0)
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

    laser_colors_init();
    laser_opts opts = laser_cli_parsecmd(argc, argv);

    // why is this here, well... so that we can free it while seing it
    if (opts.show_git)
    {
        int err = git_repository_open(&opts.git_repo, opts.dir);
        if (err != 0)
        {
            fprintf(stderr, "lsr: couldn't open git repo at '%s'\n", opts.dir);
            opts.show_git = 0;
        }
    }

    laser_list_directory(opts, 0, opts.recursive_depth);

    laser_cli_destroy_opts(opts);
    laser_colors_free();

    laser_lua_destroy();

    if (opts.git_repo)
        git_repository_free(opts.git_repo);
    git_libgit2_shutdown();

    return 0;
}
