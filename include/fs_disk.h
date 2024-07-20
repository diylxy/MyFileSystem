#ifndef __SIMPLEFS_FS_DISK_H__
#define __SIMPLEFS_FS_DISK_H__
#include <fs_common.h>
#include <fs_superblock.h>
#include <fs_block.h>
#include <fs_tree.h>

#define SIMPLE_FS_VERSION 1

FS_STATUS fs_disk_create_empty(const char *file_name, uint32_t size_in_bytes);

FS_STATUS fs_disk_format(fs_handle_t *fs, uint32_t disk_size, uint32_t block_size, const char *volume_name);

fs_handle_t *fs_disk_open(const char *device, bool *need_format);

void fs_disk_close(fs_handle_t *device_handle);

#endif