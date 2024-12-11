#include "lua_filters.h"

int lua_filters_apply(laser_opts opts, struct laser_dirent *entry)
{
    lua_getglobal(L, "L_filters");

    lua_newtable(L);

    lua_pushstring(L, entry->d->d_name);
    lua_setfield(L, -2, "name");

    lua_pushinteger(L, entry->s.st_mode);
    lua_setfield(L, -2, "mode");

    off_t size = (S_ISDIR(entry->s.st_mode)) ? -1 : entry->s.st_size;
    lua_pushinteger(L, size);
    lua_setfield(L, -2, "size");

    lua_pushinteger(L, entry->s.st_mtime);
    lua_setfield(L, -2, "mtime");

    lua_pushstring(L, getpwuid(entry->s.st_uid)->pw_name);
    lua_setfield(L, -2, "owner");

    lua_pushstring(L, S_ISDIR(entry->s.st_mode)    ? "d"
                      : S_ISLNK(entry->s.st_mode)  ? "l"
                      : S_ISCHR(entry->s.st_mode)  ? "c"
                      : S_ISBLK(entry->s.st_mode)  ? "b"
                      : S_ISFIFO(entry->s.st_mode) ? "p"
                      : S_ISSOCK(entry->s.st_mode) ? "s"
                                                   : "-");
    lua_setfield(L, -2, "type");
    for (int i = 0; i < opts.filter_count; i++)
    {
        lua_getfield(L, -2, opts.filters[i]);

        lua_pushvalue(L, -2);
        if (lua_pcall(L, 1, 1, 0) != LUA_OK)
        {
            fprintf(stderr, "lsr: error calling filter with name '%s': %s\n",
                    opts.filters[i], lua_tostring(L, -1));
            lua_pop(L, 1); // pop the error message
            exit(1);
        }
        if (!lua_isboolean(L, -1))
        {
            fprintf(stderr,
                    "lsr: filter with name '%s' did not return a boolean!\n",
                    opts.filters[i]);
            lua_pop(L, 1); // pop the error message
            exit(1);
        }

        if (lua_toboolean(L, -1))
        {
            lua_pop(L, 1); // pop the boolean
            continue;      // check the next filter
        }

        lua_pop(L, 1); // pop the boolean
        lua_pop(L, 1); // pop the table
        return 0;      // filtered out
    }

    lua_pop(L, 1); // pop the boolean
    lua_pop(L, 1); // pop the table
    return 1;
}
