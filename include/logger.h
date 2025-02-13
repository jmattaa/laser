#ifndef LASER_LOGGER_H
#define LASER_LOGGER_H

void laser_logger_log(const char *fmt, ...);
void laser_logger_error(const char *fmt, ...);
void laser_logger_fatal(int err, const char *fmt, ...);

#endif
