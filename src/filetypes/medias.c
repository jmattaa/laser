#include "filetypes/checktypes.h"

const struct laser_magicnumber laser_mediaformats[] = {
    {(unsigned char[]){0xFF, 0xD8, 0xFF, 0xE0}, 4}, // JPEG
    {(unsigned char[]){0x89, 0x50, 0x4E, 0x47}, 4}, // PNG
    {(unsigned char[]){0x47, 0x49, 0x46, 0x38}, 4}, // GIF
    {(unsigned char[]){0x49, 0x44, 0x33}, 3},       // MP3
    {(unsigned char[]){0x66, 0x74, 0x79, 0x70, 0x4D, 0x53, 0x4E, 0x56},
     3}, // MP4
    // ADD MORE MEDIA TYPES
    {NULL, 0}
};
