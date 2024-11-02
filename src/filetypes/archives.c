#include "include/checktypes.h"

const struct laser_magicnumber laser_archiveformats[] = {
    {(unsigned char[]){0x50, 0x4B, 0x03, 0x04}, 4},       // ZIP
    {(unsigned char[]){0x1F, 0x8B}, 2},                   // GZIP
    {(unsigned char[]){'u', 's', 't', 'a', 'r'}, 5},      // TAR
    {(unsigned char[]){0x42, 0x5A, 0x68}, 3},             // BZIP2
    {(unsigned char[]){0xFD, 0x37, 0x7A, 0x58, 0x5A}, 5}, // XZ
    // ADD MORE ARCHIVES
    {NULL, 0}
};
