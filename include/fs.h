#ifndef __SIMPLEFS_FS_H__
#define __SIMPLEFS_FS_H__

#include <fs_common.h>
typedef struct fs_block_description_t fs_block_description_t;
typedef struct fs_superblock_t fs_superblock_t;
typedef struct fs_handle_t
{
    fs_block_description_t *block;
    fs_superblock_t *superblock;
} fs_handle_t;

#include <fs_disk.h>
#include <fs_block_free_bitmap.h>
#include <fs_general_file.h>
#include <fs_block.h>
#include <fs_superblock.h>
#include <fs_tree.h>
#endif