#include "filetypes/checktypes.h"

const struct laser_filetype laser_documentformats[] = {
    {(unsigned char[]){'%', 'P', 'D', 'F'}, 4, (const char *[]){"pdf"},
     1}, // PDF
    {(unsigned char[]){0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1}, 8,
     (const char *[]){"doc", "xls", "ppt", "msi", "msg"},
     5}, // compound bin file from microsoft
    {(unsigned char[]){0x41, 0x54, 0x26, 0x54, 0x46, 0x4F, 0x52, 0x4D}, 8,
     (const char *[]){"djvu", "djv"}, 2}, // DJVU
    // STUFF WITHOUT MAGIC THEY SHOULD BE PLACED LAST
    {NULL, 0, (const char *[]){"html", "txt", "xml", "json"}, 4},
    // ADD DOCUMENT TYPES

    {NULL, 0, NULL, 0}};
