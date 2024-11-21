#include "main.h"
#include "colors.h"
#include "laser.h"
#include "utils.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

lua_State *L;

lua_State *initialize_lua(const char *script_path)
{
    L = luaL_newstate();
    luaL_openlibs(L);

    if (luaL_dofile(L, script_path) != LUA_OK)
    {
        fprintf(stderr, "lsr: error loading Lua script: %s\n",
                lua_tostring(L, -1));
        lua_pop(L, 1);
        return NULL;
    }

    return L;
}

int main(int argc, char **argv)
{
    const char *default_script = "lua/laser.lua";
    const char *user_script = getenv("HOME");
    char user_config_path[LASER_PATH_MAX];

    if (user_script)
        snprintf(user_config_path, sizeof(user_config_path),
                 "%s/.config/laser/laser.lua", user_script);
    else
        user_config_path[0] = '\0';

    const char *script_to_load = default_script;

    if (user_config_path[0] != '\0' && access(user_config_path, R_OK) == 0)
        script_to_load = user_config_path;

    if (!initialize_lua(script_to_load))
        return 1;

    laser_colors_init();

    laser_opts opts = laser_utils_parsecmd(argc, argv);
    laser_list_directory(opts, 0, opts.recursive_depth);

    laser_colors_free();

    return 0;
}

