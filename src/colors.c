#include "colors.h"
#include "init_lua.h"
#include <lua.h>
#include <unistd.h>

laser_color LASER_COLORS[COLOR_COUNT];

static void laser_colors_set(const char *key, const char *value)
{
    for (int i = 0; i < COLOR_COUNT; i++)
    {
        if (strcmp(LASER_COLORS[i].key, key) == 0)
        {
            LASER_COLORS[i].value = value;
            return;
        }
    }
}

void laser_colors_init(void)
{
    // we ain't printing to stdout so don't do colors
    // TODO: unless the user specifies a `--ensure-colors` flag
    int isStdout = isatty(STDOUT_FILENO);

// macro stuff be 🔥
#define _X(name, val)                                                          \
    LASER_COLORS[LASER_COLOR_##name].key = #name;                              \
    LASER_COLORS[LASER_COLOR_##name].value =                                   \
        isStdout ? val : ""; // empty of not stdout
    LASER_COLORS_ITER(_X);
#undef _X

    if (!isStdout)
        return;

    lua_getglobal(L, "L_colors");

    lua_pushnil(L);
    while (lua_next(L, -2))
    {
        const char *key = lua_tostring(L, -2);
        const char *value = lua_tostring(L, -1);

        laser_colors_set(key, value);
        lua_pop(L, 1);
    }

    lua_settop(L, 0);
}
