#include <fs_disk.h>

FS_STATUS fs_disk_create_empty(const char *file_name, uint32_t size_in_bytes)
{
    FILE *fp = fopen(file_name, "w");
    TRUE_THEN_RETURN_FALSE(fp == NULL);
    uint8_t block[512];
    memset(block, 0, sizeof(block));
    for (uint32_t i = 0; i < size_in_bytes / sizeof(block); i++)
    {
        fwrite(block, sizeof(block), 1, fp);
    }
    fclose(fp);
    return true;
}
