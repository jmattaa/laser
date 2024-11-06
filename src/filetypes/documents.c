#include "filetypes/checktypes.h"

extern const struct laser_magicnumber laser_documentformats[] = {
    {(unsigned char[]){'%', 'P', 'D', 'F'}, 4}, // PDF
    {(unsigned char[]){0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1},
     8}, // compound bin file from microsoft (doc, xls, ppt, msi, msg)
    {(unsigned char[]){0x41, 0x54, 0x26, 0x54, 0x46, 0x4F, 0x52, 0x4D},
     8}, // DJVU
    // ADD DOCUMENT TYPES
    {{NULL}, 0}};
