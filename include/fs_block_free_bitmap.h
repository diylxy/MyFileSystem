#ifndef __SIMPLEFS_FS_BLOCK_FREE_BITMAP_H__
#define __SIMPLEFS_FS_BLOCK_FREE_BITMAP_H__
#include <fs_common.h>
#include <fs_block.h>
#include <fs_superblock.h>

uint32_t fs_free_bitmap_get_total_block(fs_block_description_t *block, fs_superblock_t *superblock);

FS_STATUS fs_free_bitmap_allocate(fs_block_description_t *block, fs_superblock_t *superblock, uint32_t *result);

FS_STATUS fs_free_bitmap_free(fs_block_description_t *block, fs_superblock_t *superblock, uint32_t blocknum);

FS_STATUS fs_free_bitmap_format(fs_block_description_t *block, fs_superblock_t *superblock);

#endif