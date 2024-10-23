#ifndef LASER_GIT_UTILS_H
#define LASER_GIT_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32_t lgit_utils_hexToUint32(const char *hex);
uint32_t lgit_utils_binToUint32(const char *bin);
char *lgit_utils_readbin(char *filename, size_t *size);
char *lgit_utils_binToHex(const char *bincontent, size_t size);

char *lgit_utils_getChars(const char *str, int n, int x);
char *lgit_utils_getBinaryData(const char *buffer, size_t start, size_t length,
                               size_t total_size);

#endif
