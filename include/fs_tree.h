#ifndef __SIMPLEFS_FS_TREE_H__
#define __SIMPLEFS_FS_TREE_H__
#include <fs_common.h>

FS_STATUS fs_tree_file_create(fs_handle_t *fs, fs_tree_handle_t *parent, const char *name, fs_tree_handle_t *result);

FS_STATUS fs_tree_readdir(fs_handle_t *fs, fs_tree_handle_t *tree, fs_tree_read_result_t *result);

FS_STATUS fs_tree_append_entry(fs_handle_t *fs, fs_tree_handle_t *tree, uint32_t entry_blockid);

FS_STATUS fs_tree_remove_entry(fs_handle_t *fs, fs_tree_handle_t *tree, uint32_t entry_blockid);

FS_STATUS fs_tree_rmdir(fs_handle_t *fs, fs_tree_handle_t *tree);

bool fs_tree_exist(fs_handle_t *fs, fs_tree_handle_t *tree, const char *name);

fs_tree_handle_t *fs_tree_open(fs_handle_t *fs, const char *path);

void fs_tree_close(fs_handle_t *fs, fs_tree_handle_t *ptree);

FS_STATUS fs_tree_create(fs_handle_t *fs, const char *path);

fs_tree_handle_t *fs_tree_open_parent(fs_handle_t *fs, const char *path);

FS_STATUS fs_tree_rmdir_by_path(fs_handle_t *fs, const char *path);

#endif