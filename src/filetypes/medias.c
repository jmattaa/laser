#include "filetypes/checktypes.h"

const struct laser_filetype laser_mediaformats[] = {
    {(unsigned char[]){0xFF, 0xD8, 0xFF, 0xE0}, 4,
     (const char *[]){"jpeg", "jpg"}, 2}, // JPEG
    {(unsigned char[]){0x89, 0x50, 0x4E, 0x47}, 4, (const char *[]){"png"},
     1}, // PNG
    {(unsigned char[]){0x47, 0x49, 0x46, 0x38}, 4, (const char *[]){"gif"},
     1},                                                                  // GIF
    {(unsigned char[]){0x49, 0x44, 0x33}, 3, (const char *[]){"mp3"}, 1}, // MP3
    {(unsigned char[]){0x66, 0x74, 0x79, 0x70, 0x4D, 0x53, 0x4E, 0x56}, 8,
     (const char *[]){"mp4"}, 1}, // MP4
    // ADD MORE MEDIA TYPES

    {NULL, 0, NULL, 0}};
