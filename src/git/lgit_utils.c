#include "git/lgit_utils.h"

uint32_t lgit_utils_hexToUint32(const char *hex)
{
    return (uint32_t)strtoul(hex, NULL, 16);
}

uint32_t lgit_utils_binToUint32(const char *bin)
{
    return (int)strtol(bin, NULL, 2);
}

char *lgit_utils_readbin(char *filename, size_t *size)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        perror("Unable to open file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (fileSize <= 0)
    {
        fclose(file);
        return NULL;
    }

    char *buffer = (char *)malloc(fileSize + 1);
    if (!buffer)
    {
        fclose(file);
        perror("Memory allocation failed");
        return NULL;
    }

    size_t bytesRead = fread(buffer, 1, fileSize, file);
    if (bytesRead != fileSize)
    {
        free(buffer);
        fclose(file);
        perror("Error reading file");
        return NULL;
    }

    buffer[fileSize] = '\0';

    fclose(file);
    if (size)
    {
        *size = fileSize;
    }

    return buffer;
}

char *lgit_utils_binToHex(const char *bincontent, size_t size)
{
    // 1 byte = 2 hex
    // then + null terminator
    char *hexString = (char *)malloc(size * 2 + 1);
    if (!hexString)
    {
        perror("Memory allocation failed");
        return NULL;
    }

    for (size_t i = 0; i < size; ++i)
        sprintf(hexString + i * 2, "%02X", (unsigned char)bincontent[i]);

    hexString[size * 2] = '\0';
    return hexString;
}

char *lgit_utils_getChars(const char *str, int n, int x)
{
    int length = strlen(str);

    if (n < 0 || n >= length)
    {
        printf("Index n (%d) is out of bounds.\n", n);
        return NULL;
    }

    if (n + x > length)
    {
        x = length - n;
    }

    char *result = (char *)malloc((x + 1) * sizeof(char));
    if (result == NULL)
    {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    strncpy(result, str + n, x);
    result[x] = '\0';

    return result;
}

char *lgit_utils_getBinaryData(const char *buffer, size_t start, size_t length,
                               size_t total_size)
{
    if (start < 0 || start >= total_size || start + length > total_size)
    {
        printf(
            "Start (%zu) and length (%zu) out of bounds (total size: %zu).\n",
            start, length, total_size);
        return NULL;
    }

    char *result = (char *)malloc(length * sizeof(char));
    if (result == NULL)
    {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    memcpy(result, buffer + start, length);

    return result;
}
