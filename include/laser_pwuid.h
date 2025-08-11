#ifndef LASER_PWUID_H
#define LASER_PWUID_H

#include <pwd.h>

struct laser_uid
{
    uid_t uid;
    char *name; // we don need the whole passwd struct
};

// i doubt you have more than this many users on your system
#define LASER_UID_CACHE_SIZE 64
struct laser_uid_cache
{
    struct laser_uid uid_cache[LASER_UID_CACHE_SIZE];
    size_t uid_cache_count;
};

struct laser_uid *laser_getpwuid(uid_t uid);
void laser_pwuid_free_cache(void);

#endif
