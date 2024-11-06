#include "colors.h"

laser_colors *LASER_COLORS;

void laser_colors_init(void)
{
    LASER_COLORS = malloc(sizeof(laser_colors));

    LASER_COLORS->reset = strdup(LASER_DEFAULT_RESET_COLOR);
    LASER_COLORS->dir = strdup(LASER_DEFAULT_DIR_COLOR);
    LASER_COLORS->symlink = strdup(LASER_DEFAULT_SYMLINK_COLOR);
    LASER_COLORS->file = strdup(LASER_DEFAULT_FILE_COLOR);
    LASER_COLORS->hidden = strdup(LASER_DEFAULT_HIDDEN_COLOR);
    LASER_COLORS->exec = strdup(LASER_DEFAULT_EXEC_COLOR);
    LASER_COLORS->archive = strdup(LASER_DEFAULT_ARCHIVE_COLOR);
    LASER_COLORS->media = strdup(LASER_DEFAULT_MEDIA_COLOR);
    LASER_COLORS->documents = strdup(LASER_DEFAULT_DOCUMENT_COLOR);

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
        value = processed_value;

        // TODO: DO SOMETHING INSTEAD OF THE IF-ELSE AND STRCMP
        if (strcmp(key, "RESET") == 0)
            LASER_COLORS->reset = value;
        else if (strcmp(key, "DIR") == 0)
            LASER_COLORS->dir = value;
        else if (strcmp(key, "SYMLINK") == 0)
            LASER_COLORS->symlink = value;
        else if (strcmp(key, "FILE") == 0)
            LASER_COLORS->file = value;
        else if (strcmp(key, "HIDDEN") == 0)
            LASER_COLORS->hidden = value;
        else if (strcmp(key, "EXEC") == 0)
            LASER_COLORS->exec = value;
        else if (strcmp(key, "ARCHIVE") == 0)
            LASER_COLORS->archive = value;
        else if (strcmp(key, "MEDIA") == 0)
            LASER_COLORS->media = value;
        else if (strcmp(key, "DOCUMENT") == 0)
            LASER_COLORS->documents = value;
    }
}

void laser_colors_destroy(void)
{
    free((void *)LASER_COLORS->reset);
    free((void *)LASER_COLORS->dir);
    free((void *)LASER_COLORS->symlink);
    free((void *)LASER_COLORS->file);
    free((void *)LASER_COLORS->hidden);
    free((void *)LASER_COLORS->exec);
    free((void *)LASER_COLORS->archive);
    free((void *)LASER_COLORS->media);
    free((void *)LASER_COLORS->documents);

    free(LASER_COLORS);
}
