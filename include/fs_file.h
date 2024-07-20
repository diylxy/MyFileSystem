#ifndef __SIMPLEFS_FS_FILE_H__
#define __SIMPLEFS_FS_FILE_H__
#include <fs_typedefs.h>
#include <fs_block.h>
#include <fs_superblock.h>
#include <fs_general_file.h>
#include <fs_tree.h>


FS_STATUS fs_file_create(fs_handle_t *fs, const char *path);

fs_general_file_handle_t *fs_general_file_open_by_path(fs_handle_t *fs, fs_tree_handle_t *parent, const char *path);

fs_file_handle_t *fs_file_open(fs_handle_t *fs, const char *path);

uint32_t fs_file_write(fs_handle_t *fs, fs_file_handle_t *fp, void *buffer, uint32_t size);

uint32_t fs_file_read(fs_handle_t *fs, fs_file_handle_t *fp, void *buffer, uint32_t size);

FS_STATUS fs_file_seek(fs_handle_t *fs, fs_file_handle_t *fp, int32_t offset, int seek_method);

void fs_file_close(fs_handle_t *fs, fs_file_handle_t *fp);

FS_STATUS fs_file_remove(fs_handle_t *fs, fs_tree_handle_t *parent, fs_file_handle_t *file);

FS_STATUS fs_file_remove_by_path(fs_handle_t *fs, const char *path);

#endif