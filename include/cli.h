#ifndef LASER_CLI_H
#define LASER_CLI_H

#include "utils.h"
#include <stdio.h>

#define LASER_VERSION "1.0.0"

laser_opts laser_cli_parsecmd(int argc, char **argv);
void laser_cli_generate_completions(const char *shell,
                                    struct option *long_args);

#endif
