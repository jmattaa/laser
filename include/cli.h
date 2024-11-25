#ifndef LASER_CLI_H
#define LASER_CLI_H

#include "utils.h"
#include <stdio.h>

#define LASER_VERSION "1.0.2"

laser_opts laser_cli_parsecmd(int argc, char **argv);
void laser_cli_generate_completions(const char *shell);
void laser_cli_print_help(void);

#endif
