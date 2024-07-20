#ifndef __SIMPLEFS_FS_TYPEDEFS_H__
#define __SIMPLEFS_FS_TYPEDEFS_H__

#include <stdbool.h>
#include <stdint.h>

typedef struct fs_block_description_t
{
    FILE *fp;
    uint32_t blocksize;
    uint32_t current_block;
    uint8_t *current_block_data;
    uint8_t *__write_cache;
    int64_t __write_cache_block;
}fs_block_description_t;

typedef struct fs_superblock_t
{
    uint32_t crc32;        // 当前块的CRC（固定）
    uint32_t magic;       // 超级块识别头
    uint32_t version;      // 文件系统版本
    uint32_t block_total;   // 总块数
    uint32_t block_size;    // 块大小（字节）
    uint32_t first_block;  // 根目录块号
    char volume_name[256]; // 卷标，UTF-8编码
    // 之后全0
} fs_superblock_t;

typedef struct fs_handle_t
{
    fs_block_description_t *block;
    fs_superblock_t *superblock;
} fs_handle_t;

///////////////////////////////////////////////////////////////////////////////////////////////////

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
    bool changed;
} fs_general_file_handle_t;

///////////////////////////////////////////////////////////////////////////////////////////////////

typedef fs_general_file_handle_t fs_file_handle_t;

typedef fs_general_file_handle_t fs_tree_handle_t;

typedef struct fs_tree_read_result_t
{
    fs_general_file_handle_t sub_file_handle;
    char name[256];
    bool is_dir;
} fs_tree_read_result_t;

#endif