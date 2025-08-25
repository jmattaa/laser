#include "lua_filters.h"
#include "laser_pwuid.h"
#include "logger.h"
#include <sys/stat.h>
#include <unistd.h>

static void ensure_stat(struct laser_dirent *entry, const char *full_path);

// lazy getters

static int lua_get_mode(lua_State *L)
{
    struct laser_dirent *entry = lua_touserdata(L, lua_upvalueindex(1));
    const char *full_path = lua_tostring(L, lua_upvalueindex(2));
    ensure_stat(entry, full_path);
    lua_pushinteger(L, entry->s.st_mode);
    return 1;
}

static int lua_get_size(lua_State *L)
{
    struct laser_dirent *entry = lua_touserdata(L, lua_upvalueindex(1));
    const char *full_path = lua_tostring(L, lua_upvalueindex(2));
    ensure_stat(entry, full_path);
    off_t size = (S_ISDIR(entry->s.st_mode)) ? -1 : entry->s.st_size;
    lua_pushinteger(L, size);
    return 1;
}

static int lua_get_mtime(lua_State *L)
{
    struct laser_dirent *entry = lua_touserdata(L, lua_upvalueindex(1));
    const char *full_path = lua_tostring(L, lua_upvalueindex(2));
    ensure_stat(entry, full_path);
    lua_pushinteger(L, entry->s.st_mtime);
    return 1;
}

static int lua_get_owner(lua_State *L)
{
    struct laser_dirent *entry = lua_touserdata(L, lua_upvalueindex(1));
    const char *full_path = lua_tostring(L, lua_upvalueindex(2));
    ensure_stat(entry, full_path);
    lua_pushstring(L, laser_getpwuid(entry->s.st_uid)->name);
    return 1;
}

static int lua_get_type(lua_State *L)
{
    struct laser_dirent *entry = lua_touserdata(L, lua_upvalueindex(1));
    const char *full_path = lua_tostring(L, lua_upvalueindex(2));
    ensure_stat(entry, full_path);
    lua_pushstring(L, S_ISDIR(entry->s.st_mode)    ? "d"
                      : S_ISLNK(entry->s.st_mode)  ? "l"
                      : S_ISCHR(entry->s.st_mode)  ? "c"
                      : S_ISBLK(entry->s.st_mode)  ? "b"
                      : S_ISFIFO(entry->s.st_mode) ? "p"
                      : S_ISSOCK(entry->s.st_mode) ? "s"
                                                   : "-");
    return 1;
}

// --- Stat loader ---
static void ensure_stat(struct laser_dirent *entry, const char *full_path)
{
    if (!entry->stat_loaded)
    {
        if (lstat(full_path, &entry->s) == -1)
            luaL_error(L, "stat failed for %s", full_path);
        entry->stat_loaded = 1;
    }
}

// --- Main filter application ---
int lua_filters_apply(laser_opts opts, struct laser_dirent *entry,
                      const char *full_path)
{
    lua_getglobal(L, "L_filters");

    lua_newtable(L);

    lua_pushstring(L, entry->d->d_name);
    lua_setfield(L, -2, "name");

    lua_pushstring(
        L, (char[]){entry->git_status == ' ' ? 0 : entry->git_status, 0});
    lua_setfield(L, -2, "git_status");

    // lazy fields
    lua_pushlightuserdata(L, entry);
    lua_pushstring(L, full_path);
    lua_pushcclosure(L, lua_get_mode, 2);
    lua_setfield(L, -2, "mode");

    lua_pushlightuserdata(L, entry);
    lua_pushstring(L, full_path);
    lua_pushcclosure(L, lua_get_size, 2);
    lua_setfield(L, -2, "size");

    lua_pushlightuserdata(L, entry);
    lua_pushstring(L, full_path);
    lua_pushcclosure(L, lua_get_mtime, 2);
    lua_setfield(L, -2, "mtime");

    lua_pushlightuserdata(L, entry);
    lua_pushstring(L, full_path);
    lua_pushcclosure(L, lua_get_owner, 2);
    lua_setfield(L, -2, "owner");

    lua_pushlightuserdata(L, entry);
    lua_pushstring(L, full_path);
    lua_pushcclosure(L, lua_get_type, 2);
    lua_setfield(L, -2, "type");

    // run the filters
    for (int i = 0; i < opts.filter_count; i++)
    {
        lua_getfield(L, -2, opts.filters[i]);
        lua_pushvalue(L, -2); // push the file table

        if (lua_pcall(L, 1, 1, 0) != LUA_OK)
        {
            laser_logger_error("error calling filter '%s': %s\n",
                               opts.filters[i], lua_tostring(L, -1));
            lua_pop(L, 1);
            exit(1);
        }

        if (!lua_isboolean(L, -1))
        {
            laser_logger_error("filter '%s' did not return a boolean!\n",
                               opts.filters[i]);
            lua_pop(L, 1);
            exit(1);
        }

        if (lua_toboolean(L, -1))
        {
            lua_pop(L, 1); // pop boolean
            continue;
        }

        lua_pop(L, 1); // pop boolean
        lua_pop(L, 1); // pop table
        return 0;
    }

    lua_pop(L, 1); // pop last boolean
    lua_pop(L, 1); // pop table
    return 1;
}
