#ifndef LASER_LUA_FILTERS_H
#define LASER_LUA_FILTERS_H

#include "init_lua.h"
#include "laser.h"

int lua_filters_apply(laser_opts opts, struct laser_dirent *entry,
                      const char *fp);

#endif
