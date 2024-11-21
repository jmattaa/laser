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
        char *token = strtok(colors_copy, ":");

        while (token != NULL)
        {
            laser_colors_parseToken(token);
            token = strtok(NULL, ":");
        }
    }
}

void laser_colors_parseToken(const char *token)
{
    char *separator = strchr(token, '=');

    if (separator != NULL)
    {
        *separator = '\0';
        const char *key = token;
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

        for (int i = 0; i < COLOR_COUNT; i++)
        {
            if (strcmp(key, LASER_COLORS[i].key) == 0)
            {
                free((void *)LASER_COLORS[i].value); // free old default val
                LASER_COLORS[i].value = processed_value;
                break;
            }
        }
    }
}

void laser_colors_free(void)
{
    for (int i = 0; i < COLOR_COUNT; i++)
    {
        free((void *)LASER_COLORS[i].key);
        free((void *)LASER_COLORS[i].value);
    }

    free(LASER_COLORS);
}
