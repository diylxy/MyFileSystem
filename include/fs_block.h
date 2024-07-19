#ifndef __SIMPLEFS_FS_BLOCK_H__
#define __SIMPLEFS_FS_BLOCK_H__
#include <fs_common.h>
#include <fs_crc32.h>
typedef struct fs_block_description_t
{
    FILE *fp;
    uint32_t blocksize;
    uint32_t current_block;
    uint8_t *current_block_data;
    uint8_t *__write_cache;
    int64_t __write_cache_block;
}fs_block_description_t;


FS_STATUS fs_block_open(fs_block_description_t *block, char *filename, uint32_t blocksize);

FS_STATUS fs_block_reset_blocksize(fs_block_description_t *block, uint32_t blocksize);

FS_STATUS fs_block_close(fs_block_description_t *block);

FS_STATUS fs_block_read(fs_block_description_t *block, uint32_t target_block);

FS_STATUS fs_block_read_no_read_cache(fs_block_description_t *block, uint32_t target_block);

FS_STATUS fs_block_write(fs_block_description_t *block, uint32_t target_block);

FS_STATUS fs_block_sync(fs_block_description_t *block);

#endif