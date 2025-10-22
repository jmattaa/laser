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
    {(unsigned char[]){0x52, 0x49, 0x46, 0x46}, 4,
     (const char *[]){"wav", "webp"}, 1}, // WAV and WEBP
    {(unsigned char[]){0x66, 0x74, 0x79, 0x70, 0x68, 0x65, 0x69, 0x63, 0x66,
                       0x74, 0x79, 0x70, 0x6d},
     13, (const char *[]){"heic"}, 1}, // HEIC
    // STUFF WITHOUT MAGIC THEY SHOULD BE PLACED LAST
    {NULL, 0, (const char *[]){"mov", "svg", "ico", "avif"}, 4}, 
    // ADD MORE MEDIA TYPES

    {NULL, 0, NULL, 0}};
