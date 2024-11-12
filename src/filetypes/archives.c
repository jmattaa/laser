#include "filetypes/checktypes.h"

const struct laser_format_token laser_archiveformats[] = {
    {(unsigned char[]){0x50, 0x4B, 0x03, 0x04}, 4}, // ZIP and stuff based on it
    {(unsigned char[]){0x1F, 0x8B}, 2},             // GZIP
    {(unsigned char[]){'u', 's', 't', 'a', 'r'}, 5},      // TAR
    {(unsigned char[]){0x42, 0x5A, 0x68}, 3},             // BZIP2
    {(unsigned char[]){0xFD, 0x37, 0x7A, 0x58, 0x5A}, 5}, // XZ
    {(unsigned char[]){0x52, 0x61, 0x72, 0x21, 0x1A, 0x07, 0x00},
     7}, // RAR v1.5 to 4.0
    {(unsigned char[]){0x52, 0x61, 0x72, 0x21, 0x1A, 0x07, 0x01, 0x00},
     8}, // RAR v5+
    // ADD MORE ARCHIVES
    {NULL, 0}};
