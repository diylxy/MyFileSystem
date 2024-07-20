#ifndef __SIMPLEFS_FS_SUPERBLOCK_H__
#define __SIMPLEFS_FS_SUPERBLOCK_H__
#include <fs_common.h>
#include <fs_block.h>

FS_STATUS fs_superblock_getinfo(fs_block_description_t *block, fs_superblock_t *info);

FS_STATUS fs_superblock_update(fs_block_description_t *block, fs_superblock_t *info);

#endif