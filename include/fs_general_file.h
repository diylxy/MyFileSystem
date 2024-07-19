#ifndef __SIMPLEFS_FS_GENERAL_FILE_H__
#define __SIMPLEFS_FS_GENERAL_FILE_H__
#include <fs_common.h>
#include <fs_block.h>
#include <fs_superblock.h>
#include <fs_block_free_bitmap.h>

typedef struct fs_general_file_block_header_t // 一般文件块头
{
    uint32_t crc32;          // 当前块的CRC（固定）
    uint32_t block_previous; // 上一个块的块号（0表示这是该文件的第一个块）
    uint32_t block_next;     // 下一个块的块号（0表示这是该文件的最后一个块）
    // 之后是文件内容，如果是第一块，还需要下面定义的文件头
} fs_general_file_block_header_t;

typedef struct fs_general_file_header_t // 文件头
{
    uint16_t magic;       // 表示该块是目录还是文件
    uint16_t name_length; // 文件名长度
    uint32_t file_size;   // 文件大小
    uint32_t create_time; // 文件创建日期
    uint32_t modify_time; // 文件修改日期
    // 之后存储文件名（注意检查最长256字节）
    // 之后存储文件内容
} fs_general_file_header_t;

typedef struct fs_general_file_handle_t // 文件句柄
{
    fs_general_file_header_t header; // 文件头
    uint32_t block_first;            // 文件第一个块号
    uint32_t block_current;          // 当前读写的物理块号
    uint32_t block_offset;           // 当前读写的块内偏移
    uint32_t pos_current;            // 当前读写的位置(相对于文件第一个字节)
} fs_general_file_handle_t;


FS_STATUS fs_general_file_open(fs_block_description_t *block, fs_superblock_t *superblock, fs_general_file_handle_t *handle, uint32_t blocknum);

FS_STATUS fs_general_file_create(fs_block_description_t *block, fs_superblock_t *superblock, const char *file_name, uint16_t magic, uint32_t *block_first);

FS_STATUS fs_general_file_get_filename(fs_block_description_t *block, fs_general_file_handle_t *handle, char *buffer, uint32_t max_chr_count);

uint32_t fs_general_file_read(fs_block_description_t *block, fs_superblock_t *superblock, fs_general_file_handle_t *handle, void *buffer, uint32_t size);

uint32_t fs_general_file_write(fs_block_description_t *block, fs_superblock_t *superblock, fs_general_file_handle_t *handle, const void *buffer, uint32_t size);

FS_STATUS fs_general_file_seek(fs_block_description_t *block, fs_general_file_handle_t *handle, int32_t offset, int seek_method);

FS_STATUS fs_general_file_sync(fs_block_description_t *block, fs_general_file_handle_t *handle);

FS_STATUS fs_general_file_close(fs_block_description_t *block, fs_general_file_handle_t *handle);

FS_STATUS fs_general_file_remove(fs_block_description_t *block, fs_superblock_t *superblock, uint32_t blocknum);

FS_STATUS fs_general_file_trim_size_to_current_position(fs_block_description_t *block, fs_superblock_t *superblock, fs_general_file_handle_t *handle);

#endif