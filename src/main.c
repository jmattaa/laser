#include "main.h"
#include "colors.h"
#include "laser.h"
#include "utils.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

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
    if (!initialize_lua("lua/laser.lua"))
    {
        fprintf(stderr, "lsr: lua error\n");
        return 1;
    }

    laser_colors_init();

    laser_opts opts = laser_utils_parsecmd(argc, argv);
    laser_list_directory(opts, 0, opts.recursive_depth);
    return 0;
}
