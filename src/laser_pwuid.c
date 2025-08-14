#include "laser_pwuid.h"
#include "logger.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

static struct laser_uid_cache uid_cache = {.uid_cache_count = 0};

// to free the allocated memory by the cache you have to call the
// `laser_pwuid_free_cache` function at the end of the program run, you 
// SHOULDN'T be manually freeing this memory!!
struct laser_uid *laser_getpwuid(uid_t uid)
{
    for (size_t i = 0; i < uid_cache.uid_cache_count; i++)
    {
        if (uid_cache.uid_cache[i].uid == uid)
            return &uid_cache.uid_cache[i];
    }

    struct passwd *pw = getpwuid(uid);
    if (!pw)
        laser_logger_fatal(1, "getpwuid failed: %s", strerror(errno));

    // if there is not space (which shouldn't happen cuz if it does then that's
    // impressive, how many users have you got???) then just return NULL
    if (uid_cache.uid_cache_count >= LASER_UID_CACHE_SIZE)
        return NULL;

    struct laser_uid *entry = &uid_cache.uid_cache[uid_cache.uid_cache_count];
    uid_cache.uid_cache_count++;

    entry->uid = uid;
    entry->name = strdup(pw->pw_name);
    if (!entry->name)
        laser_logger_fatal(1, "failed to allocate memory for uid name: %s",
                           strerror(errno));

    return entry;
}

void laser_pwuid_free_cache(void)
{
    for (size_t i = 0; i < uid_cache.uid_cache_count; i++)
    {
        if (uid_cache.uid_cache[i].name)
            free(uid_cache.uid_cache[i].name);
    }

    uid_cache.uid_cache_count = 0;
}
