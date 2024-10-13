#include "include/laser.h"
#include "include/utils.h"

int main(int argc, char **argv)
{
    laser_opts opts = laser_utils_parsecmd(argc, argv);

    laser_dir_entries lentries = laser_getdirs(opts);
    laser_listdirs(lentries);

    return 0;
}
