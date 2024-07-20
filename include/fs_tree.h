#ifndef __SIMPLEFS_FS_TREE_H__
#define __SIMPLEFS_FS_TREE_H__
#include <fs_block.h>
#include <fs_superblock.h>
#include <fs_general_file.h>

typedef struct fs_tree_read_result_t
{
    fs_general_file_handle_t sub_file_handle;
    char name[256];
    bool is_dir;
} fs_tree_read_result_t;

typedef fs_general_file_handle_t fs_tree_handle_t;

FS_STATUS fs_tree_file_create(fs_handle_t *fs, fs_tree_handle_t *parent, const char *name, fs_tree_handle_t *result);

FS_STATUS fs_tree_readdir(fs_handle_t *fs, fs_tree_handle_t *tree, fs_tree_read_result_t *result);

FS_STATUS fs_tree_append_entry(fs_handle_t *fs, fs_tree_handle_t *tree, uint32_t entry_blockid);

FS_STATUS fs_tree_remove_entry(fs_handle_t *fs, fs_tree_handle_t *tree, uint32_t entry_blockid);

FS_STATUS fs_tree_rmdir(fs_handle_t *fs, fs_tree_handle_t *tree);

bool fs_tree_exist(fs_handle_t *fs, fs_tree_handle_t *tree, const char *name);

fs_tree_handle_t *fs_tree_open(fs_handle_t *fs, const char *path);

void fs_tree_close(fs_handle_t *fs, fs_tree_handle_t *ptree);

FS_STATUS fs_tree_create(fs_handle_t *fs, const char *path);

#endif