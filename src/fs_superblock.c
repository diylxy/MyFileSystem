#include <fs_superblock.h>

FS_STATUS fs_superblock_getinfo(fs_block_description_t *block, fs_superblock_t *info)
{
    uint8_t *data;
    uint32_t crc32_origin;
    uint32_t crc32_calculated;
    TRUE_THEN_RETURN_FALSE(block == NULL);
    fs_block_read_no_read_cache(block, 0);                // 此处不要校验
    memcpy(info, block->current_block_data, sizeof(fs_superblock_t));
    if(info->magic != FS_SUPERBLOCK_HEADER) return false;
    return true;
}

FS_STATUS fs_superblock_update(fs_block_description_t *block, fs_superblock_t *info)
{
    TRUE_THEN_RETURN_FALSE(block == NULL);
    memcpy(block->current_block_data, info, sizeof(fs_superblock_t));
    info->magic = FS_SUPERBLOCK_HEADER;
    fs_block_write(block, 0);
    return true;
}
