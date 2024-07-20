#include <fs_block.h>

FS_STATUS fs_block_check_crc32(fs_block_description_t *block)
{
    uint32_t crc32_origin;
    uint32_t crc32_calculated;
    TRUE_THEN_RETURN_FALSE(block == NULL);
    crc32_origin = *(uint32_t *)(block->current_block_data);
    *(uint32_t *)(block->current_block_data) = 0;
    crc32_calculated = crc32(block->current_block_data, block->blocksize);
    *(uint32_t *)(block->current_block_data) = crc32_origin;
    if(crc32_origin != crc32_calculated)
    {
        return false;
    }
    return true;
}

FS_STATUS fs_block_fill_crc32(fs_block_description_t *block)
{
    TRUE_THEN_RETURN_FALSE(block == NULL);
    *(uint32_t *)(block->current_block_data) = 0;
    *(uint32_t *)(block->current_block_data) = crc32(block->current_block_data, block->blocksize);
    return true;
}

FS_STATUS fs_block_open(fs_block_description_t *block, char *filename, uint32_t blocksize)
{
    TRUE_THEN_RETURN_FALSE(block == NULL);
    block->fp = fopen(filename, "r+");
    TRUE_THEN_RETURN_FALSE(block->fp == NULL);
    block->blocksize = blocksize;
    block->current_block = 0;
    block->current_block_data = (uint8_t *)malloc(blocksize);
    block->__write_cache = (uint8_t *)malloc(blocksize);
    block->__write_cache_block = -1;
    TRUE_THEN_RETURN_FALSE(block->current_block_data == NULL);
    fread(block->current_block_data, block->blocksize, 1, block->fp);
    block->current_block = 0;
    if(fs_block_check_crc32(block) == false) return false;
    return true;
}

FS_STATUS fs_block_reset_blocksize(fs_block_description_t *block, uint32_t blocksize)
{
    TRUE_THEN_RETURN_FALSE(block == NULL);
    free(block->current_block_data);
    free(block->__write_cache);
    block->blocksize = blocksize;
    block->current_block = 0;
    block->current_block_data = (uint8_t *)malloc(blocksize);
    block->__write_cache = (uint8_t *)malloc(blocksize);
    block->__write_cache_block = -1;
    fseek(block->fp, 0, SEEK_SET);
    fread(block->current_block_data, block->blocksize, 1, block->fp);
    block->current_block = 0;
    if(fs_block_check_crc32(block) == false) return false;
    return true;
}

FS_STATUS fs_block_close(fs_block_description_t *block)
{
    TRUE_THEN_RETURN_FALSE(block == NULL);
    if(block->__write_cache_block != -1)
    {
        fseek(block->fp, block->__write_cache_block * block->blocksize, SEEK_SET);
        fwrite(block->__write_cache, block->blocksize, 1, block->fp);
        printf("==> W%d\n", block->__write_cache_block);
        block->__write_cache_block = -1;
    }
    fclose(block->fp);
    free(block->current_block_data);
    free(block->__write_cache);
    return true;
}

FS_STATUS fs_block_read(fs_block_description_t *block, uint32_t target_block)
{
    TRUE_THEN_RETURN_FALSE(block == NULL);
    if(block->__write_cache_block == target_block)
    {
        memcpy(block->current_block_data, block->__write_cache, block->blocksize);           // 直接读取写入缓存
        block->current_block = block->__write_cache_block;
        return true;
    }
    if(block->current_block == target_block)
        return true;
    fseek(block->fp, target_block * block->blocksize, SEEK_SET);
    fread(block->current_block_data, block->blocksize, 1, block->fp);
    block->current_block = target_block;
    printf("<== R %d\n", target_block);
    TRUE_THEN_RETURN_FALSE(fs_block_check_crc32(block) == false);
    return true;
}

FS_STATUS fs_block_read_no_read_cache(fs_block_description_t *block, uint32_t target_block)
{
    TRUE_THEN_RETURN_FALSE(block == NULL);
    if(block->__write_cache_block == target_block)
    {
        memcpy(block->current_block_data, block->__write_cache, block->blocksize);           // 直接读取写入缓存
        block->current_block = block->__write_cache_block;
        return true;
    }
    fseek(block->fp, target_block * block->blocksize, SEEK_SET);
    fread(block->current_block_data, block->blocksize, 1, block->fp);
    block->current_block = target_block;
    printf("<== R [F]%d\n", target_block);
    if(fs_block_check_crc32(block) == false) return false;
    return true;
}


FS_STATUS fs_block_write(fs_block_description_t *block, uint32_t target_block)
{
    TRUE_THEN_RETURN_FALSE(block == NULL);
    fs_block_fill_crc32(block);
    if(target_block == 0)
    {
        printf("警告: 正在修改超级块\n");
    }
    if(block->__write_cache_block != -1 && block->__write_cache_block != target_block)
    {
        fseek(block->fp, block->__write_cache_block * block->blocksize, SEEK_SET);
        fwrite(block->__write_cache, block->blocksize, 1, block->fp);
        printf("==> W%d\n", block->__write_cache_block);
    }
    block->__write_cache_block = target_block;
    memcpy(block->__write_cache, block->current_block_data, block->blocksize);
    block->current_block = target_block;
    return true;
}

FS_STATUS fs_block_sync(fs_block_description_t *block)
{
    TRUE_THEN_RETURN_FALSE(block == NULL);
    if(block->__write_cache_block != -1)
    {
        fseek(block->fp, block->__write_cache_block * block->blocksize, SEEK_SET);
        fwrite(block->__write_cache, block->blocksize, 1, block->fp);
        printf("==> W%d\n", block->__write_cache_block);
        block->__write_cache_block = -1;
    }
    return true;
}
