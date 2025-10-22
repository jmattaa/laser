#include "filetypes/checktypes.h"

const struct laser_filetype laser_archiveformats[] = {

    {
        (unsigned char[]){0x50, 0x4B, 0x03, 0x04},
        4,
        (const char *[]){"zip", "aar",  "apk",  "docx", "epub", "ipa", "jar",
                         "kmz", "maff", "msix", "odp",  "ods",  "odt", "pk3",
                         "pk4", "pptx", "usdz", "vsdx", "xlsx", "xpi"},
        20,
    }, // ZIP and stuff based on it
    {(unsigned char[]){0x1F, 0x8B}, 2, (const char *[]){"gz"}, 1}, // GZIP
    {(unsigned char[]){'u', 's', 't', 'a', 'r'}, 5, (const char *[]){"tar"},
     1}, // TAR
    {(unsigned char[]){0x42, 0x5A, 0x68}, 3, (const char *[]){"bz2"},
     1}, // BZIP2
    {(unsigned char[]){0xFD, 0x37, 0x7A, 0x58, 0x5A}, 5, (const char *[]){"xz"},
     1}, // XZ
    {(unsigned char[]){0x52, 0x61, 0x72, 0x21, 0x1A, 0x07, 0x00}, 7,
     (const char *[]){"rar"}, 1}, // RAR v1.5 to 4.0
    {(unsigned char[]){0x52, 0x61, 0x72, 0x21, 0x1A, 0x07, 0x01, 0x00}, 8,
     (const char *[]){"rar"}, 1}, // RAR v5+

    // STUFF WITHOUT MAGIC THEY SHOULD BE PLACED LAST
    {NULL, 0, (const char *[]){"dmg"}, 1},
    // ADD MORE ARCHIVES

    {NULL, 0, NULL, 0}};
