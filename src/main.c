#include "main.h"
#include "cli.h"
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

// macro stuff be 🔥
#define _X(name, _)                                                            \
    lua_pushstring(L, LASER_COLORS[LASER_COLOR_##name].value);                 \
    lua_setfield(L, -2, #name);

lua_State *initialize_lua()
{
    L = luaL_newstate();
    luaL_openlibs(L);

    return L;
}

void lua_load_script(const char *script_path)
{
    char script_dir[LASER_PATH_MAX];
    // set the package.path so that user can do relative requires
    const char *last_slash = strrchr(script_path, '/');
    if (last_slash != NULL)
    {
        size_t len = last_slash - script_path;
        strncpy(script_dir, script_path, len);
        script_dir[len] = '\0';

        lua_getglobal(L, "package");
        lua_getfield(L, -1, "path");
        const char *current_path = lua_tostring(L, -1);

        char new_path[LASER_PATH_MAX];
        snprintf(new_path, sizeof(new_path), "%s;%s/?.lua", current_path,
                 script_dir);
        lua_pop(L, 1);

        lua_pushstring(L, new_path);
        lua_setfield(L, -2, "path");
        lua_pop(L, 1);
    }

    // set the global colors table
    lua_newtable(L);
    LASER_COLORS_ITER(_X)
    lua_setglobal(L, "L_colors");

    if (luaL_dofile(L, script_path) != LUA_OK)
    {
        fprintf(stderr, "lsr: error loading Lua script: %s\n",
                lua_tostring(L, -1));
        lua_pop(L, 1);
    }
}

int main(int argc, char **argv)
{
    laser_colors_init(); // colors need to be initialized before lua
                         // cuz lua uses them
    if (!initialize_lua())
        return 1;

    const char *default_script = "/usr/local/share/lsr/lsr.lua";
    lua_load_script(default_script); // by loading the default script before
                                     // the eventual user script we are able to
                                     // use default functions and so on without
                                     // having to redifine

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

    if (script_to_load != default_script)
        lua_load_script(script_to_load);

    laser_opts opts = laser_cli_parsecmd(argc, argv);
    laser_list_directory(opts, 0, opts.recursive_depth);

    laser_colors_free();

    return 0;
}
