#ifndef __SIMPLEFS_FS_API_H__
#define __SIMPLEFS_FS_API_H__

#include <fs_typedefs.h>

typedef fs_handle_t simplefs_disk_handle_t;

typedef enum simplefs_open_mode_t
{
    MODE_READ,
    MODE_WRITE,
    MODE_APPEND,
} simplefs_open_mode_t;

typedef struct simplefs_file_t
{
    simplefs_disk_handle_t *disk;
    fs_file_handle_t *fp;
    bool isopened;
    simplefs_open_mode_t mode;
} simplefs_file_t;

typedef struct simplefs_dir_t
{
    simplefs_disk_handle_t *disk;
    fs_tree_handle_t *fp;
    bool isopened;
} simplefs_dir_t;

typedef fs_tree_read_result_t simplefs_tree_read_result_t;



simplefs_disk_handle_t *sfs_diskopen(const char *device, bool *need_format);

void sfs_diskclose(simplefs_disk_handle_t *device);

bool sfs_disk_create_empty(const char *disk, uint32_t disk_size);

bool sfs_diskformat(simplefs_disk_handle_t *device, uint32_t disk_size, uint32_t block_size, const char *volume_name);

bool sfs_fcreate(simplefs_disk_handle_t *drive, const char *path);

simplefs_file_t *sfs_fopen(simplefs_disk_handle_t *drive, const char *path, simplefs_open_mode_t mode);

void sfs_fclose(simplefs_file_t *f);

bool sfs_exists(simplefs_disk_handle_t *drive, const char *path);

uint32_t sfs_fread(void *buffer, uint32_t size, simplefs_file_t *fp);

uint32_t sfs_fwrite(const void *buffer, uint32_t size, simplefs_file_t *fp);

bool sfs_fseek(simplefs_file_t *fp, int32_t offset, int seek_mode);

void sfs_rewind(simplefs_file_t *fp);

bool sfs_mkdir(simplefs_disk_handle_t *drive, const char *path);

simplefs_dir_t *sfs_dir_open(simplefs_disk_handle_t *drive, const char *path);

void sfs_dir_close(simplefs_dir_t *fp);

bool sfs_tree_readdir(simplefs_dir_t *fp, simplefs_tree_read_result_t *result);

void sfs_tree_rewind(simplefs_dir_t *fp);

bool sfs_tree_rmdir(simplefs_disk_handle_t *drive, const char *path);

bool sfs_remove(simplefs_disk_handle_t *drive, const char *path);

#endif