#ifndef __SIMPLEFS_FS_DISK_H__
#define __SIMPLEFS_FS_DISK_H__
#include <fs_common.h>
#include <fs_superblock.h>

FS_STATUS fs_disk_create_empty(const char *file_name, uint32_t size_in_bytes);

#endif