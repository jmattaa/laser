#include "colors.h"
#include "init_lua.h"
#include <lua.h>

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

// macro stuff be 🔥
#define _X(name, vaule)                                                        \
    LASER_COLORS[LASER_COLOR_##name].key = #name;                      \
    LASER_COLORS[LASER_COLOR_##name].value = vaule;

void laser_colors_init(void)
{
    LASER_COLORS_ITER(_X)

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
#undef _X
