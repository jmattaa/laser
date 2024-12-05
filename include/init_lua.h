#ifndef LASER_INIT_LUA_H
#define LASER_INIT_LUA_H

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

extern lua_State *L;

lua_State *laser_init_lua(void);
void laser_lua_load_script(const char *script_path);
void laser_lua_set_package_path(const char *script_path);

#endif
