#include "init_lua.h"
#include "laser.h"
#include "logger.h"
#include <string.h>

lua_State *L;

lua_State *laser_init_lua(void)
{
    L = luaL_newstate();
    luaL_openlibs(L);

    return L;
}

void laser_lua_load_script(const char *script_path)
{
    if (luaL_dofile(L, script_path) != LUA_OK)
    {
        laser_logger_error("error loading Lua script: %s\n",
                           lua_tostring(L, -1));
        lua_pop(L, 1);
    }
}

void laser_lua_set_package_path(const char *script_path)
{
    // set the package.path so that user can do relative requires
    const char *last_slash = strrchr(script_path, '/');
    if (last_slash == NULL)
        return;

    size_t len = last_slash - script_path;
    char *script_dir = malloc(len + 1);
    strncpy(script_dir, script_path, len);
    script_dir[len] = '\0';

    lua_getglobal(L, "package");
    lua_getfield(L, -1, "path");
    const char *current_path = lua_tostring(L, -1);

    size_t nplen =
        strlen(current_path) + strlen(script_dir) + strlen(";/?.lua") + 1;
    char *new_path = malloc(nplen);
    snprintf(new_path, nplen, "%s;%s/?.lua", current_path,
             script_dir);
    lua_pop(L, 1);

    lua_pushstring(L, new_path);
    lua_setfield(L, -2, "path");
    lua_pop(L, 1);

    free(new_path);
    free(script_dir);
}

void laser_lua_destroy(void)
{
    lua_settop(L, 0);
    lua_close(L);
}
