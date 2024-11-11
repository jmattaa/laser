#include "colors.h"
#include "laser.h"
#include "utils.h"

int main(int argc, char **argv)
{
    laser_colors_init();

    laser_opts opts = laser_utils_parsecmd(argc, argv);
    laser_list_directory(opts, 0, opts.recursive_depth);
    return 0;
}
