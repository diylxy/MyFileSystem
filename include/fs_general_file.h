#ifndef __SIMPLEFS_FS_GENERAL_FILE_H__
#define __SIMPLEFS_FS_GENERAL_FILE_H__
#include <fs_common.h>
#include <fs_block.h>
#include <fs_superblock.h>
#include <fs_block_free_bitmap.h>

FS_STATUS fs_general_file_open(fs_block_description_t *block, fs_superblock_t *superblock, fs_general_file_handle_t *handle, uint32_t blocknum);

FS_STATUS fs_general_file_create(fs_block_description_t *block, fs_superblock_t *superblock, const char *file_name, uint16_t magic, uint32_t *block_first);

FS_STATUS fs_general_file_get_filename(fs_block_description_t *block, fs_general_file_handle_t *handle, char *buffer, uint32_t max_chr_count);

uint32_t fs_general_file_read(fs_block_description_t *block, fs_superblock_t *superblock, fs_general_file_handle_t *handle, void *buffer, uint32_t size);

uint32_t fs_general_file_write(fs_block_description_t *block, fs_superblock_t *superblock, fs_general_file_handle_t *handle, const void *buffer, uint32_t size);

FS_STATUS fs_general_file_rewind(fs_block_description_t *block, fs_general_file_handle_t *handle);

FS_STATUS fs_general_file_seek(fs_block_description_t *block, fs_general_file_handle_t *handle, int32_t offset, int seek_method);

FS_STATUS fs_general_file_sync(fs_block_description_t *block, fs_general_file_handle_t *handle);

FS_STATUS fs_general_file_close(fs_block_description_t *block, fs_general_file_handle_t *handle);

FS_STATUS fs_general_file_remove(fs_block_description_t *block, fs_superblock_t *superblock, uint32_t blocknum);

FS_STATUS fs_general_file_trim_size_to_current_position(fs_block_description_t *block, fs_superblock_t *superblock, fs_general_file_handle_t *handle);

#endif