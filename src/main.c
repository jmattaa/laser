#include "include/laser.h"
#include "include/utils.h"

int main(int argc, char **argv)
{
    laser_opts opts = laser_utils_parsecmd(argc, argv);
    laser_list_directory(opts, 0);
    return 0;
}
