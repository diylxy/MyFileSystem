#ifndef __SIMPLEFS_FS_SUPERBLOCK_H__
#define __SIMPLEFS_FS_SUPERBLOCK_H__
#include <fs_common.h>
#include <fs_block.h>

typedef struct fs_superblock_t
{
    uint32_t crc32;        // 当前块的CRC（固定）
    uint32_t magic;       // 超级块识别头
    uint32_t version;      // 文件系统版本
    uint32_t block_total;   // 总块数
    uint32_t first_block;  // 根目录块号
    char volume_name[256]; // 卷标，UTF-8编码
    // 之后全0
} fs_superblock_t;

FS_STATUS fs_superblock_getinfo(fs_block_description_t *block, fs_superblock_t *info);

FS_STATUS fs_superblock_update(fs_block_description_t *block, fs_superblock_t *info);

#endif