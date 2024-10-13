#include "include/utils.h"

int main(int argc, char **argv)
{
    laser_opts flags = laser_utils_parsecmd(argc, argv);

    printf("show tree: %d\nshow all: %d\n", flags.show_tree, flags.show_all);
    printf("directory: %s\n", flags.dir);

    return 0;
}
