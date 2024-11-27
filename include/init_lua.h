#ifndef LASER_INIT_LUA_H
#define LASER_INIT_LUA_H

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

extern lua_State *L;

lua_State *laser_init_lua(void);
void laser_lua_load_script(const char *script_path);
void laser_lua_set_package_path(const char *script_path);

#define laser_lua_CALL_NOARGS_NORET(func_name)                                 \
    lua_getglobal(L, func_name);                                               \
    if (lua_pcall(L, 0, 0, 0) != LUA_OK)                                       \
    {                                                                          \
        fprintf(stderr, "lsr: error in %s: %s\n", func_name,                   \
                lua_tostring(L, -1));                                          \
        lua_pop(L, 1);                                                         \
    }

#endif
