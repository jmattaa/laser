#include "colors.h"
#include "main.h"
#include <stdio.h>

laser_color *LASER_COLORS;

void laser_colors_set(const char *key, const char *value)
{
    for (int i = 0; i < COLOR_COUNT; i++)
    {
        if (strcmp(LASER_COLORS[i].key, key) == 0)
        {
            free((void *)LASER_COLORS[i].value); // free old default value

            LASER_COLORS[i].value = strdup(value);
            return;
        }
    }
}

// macro stuff be 🔥
#define _X(name, vaule)                                                        \
    LASER_COLORS[LASER_COLOR_##name].key = strdup(#name);                      \
    LASER_COLORS[LASER_COLOR_##name].value = strdup(vaule);

void laser_colors_init(void)
{
    LASER_COLORS = malloc(sizeof(laser_color) * COLOR_COUNT);
    LASER_COLORS_ITER(_X)

    lua_getglobal(L, "L_colors");
    if (!lua_istable(L, -1))
    {
        lua_pop(L, 1);
        return; // there is no L_colors use the default c colors
    }

    lua_pushnil(L);
    while (lua_next(L, -2))
    {
        const char *key = lua_tostring(L, -2);
        const char *value = lua_tostring(L, -1);

        laser_colors_set(key, value);
        lua_pop(L, 1);
    }

    lua_pop(L, 1);
}
#undef _X

#define _X(name, _)                                                            \
    free((void *)LASER_COLORS[LASER_COLOR_##name].key);                        \
    free((void *)LASER_COLORS[LASER_COLOR_##name].value);

void laser_colors_free(void)
{
    LASER_COLORS_ITER(_X)

    free(LASER_COLORS);
}
#undef _X
