#include "colors.h"
#include <stdio.h>

laser_color *LASER_COLORS;

void laser_colors_init(void)
{
    LASER_COLORS = malloc(sizeof(laser_color) * COLOR_COUNT);

    for (int i = 0; i < COLOR_COUNT; i++)
    {
        LASER_COLORS[i].key = strdup(LASER_COLORS_DEFAULTS[i].key);
        LASER_COLORS[i].value = strdup(LASER_COLORS_DEFAULTS[i].value);
    }

    const char *env_colors = getenv("LSR_COLORS");

    if (env_colors)
    {
        char *colors_copy = strdup(env_colors);
        char *signature = strtok(colors_copy, ":");

        while (signature != NULL)
        {
            laser_colors_parsesignature(signature);
            signature = strtok(NULL, ":");
        }
    }
}

void laser_colors_parsesignature(const char *signature)
{
    char *separator = strchr(signature, '=');

    if (separator != NULL)
    {
        *separator = '\0';
        const char *key = signature;
        const char *value = separator + 1;

        char *processed_value = malloc(strlen(value) + 1);
        char *dest = processed_value;

        // change a literal "\x1b" to '\x1b'
        while (*value)
        {
            if (value[0] == '\\' && value[1] == 'x' && value[2] == '1' &&
                value[3] == 'b')
            {
                *dest++ = '\x1b';
                value += 4;
            }
            else
                *dest++ = *value++;
        }
        *dest = '\0';
        value = processed_value;

        for (int i = 0; i < COLOR_COUNT; i++)
        {
            if (strcmp(key, LASER_COLORS[i].key) == 0)
            {
                LASER_COLORS[i].value = value;
                break;
            }
        }
    }
}

void laser_colors_destroy(void)
{
    for (int i = 0; i < COLOR_COUNT; i++)
    {
        free((void *)LASER_COLORS[i].value);
        free((void *)LASER_COLORS[i].key);
    }

    free(LASER_COLORS);
}
