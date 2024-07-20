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

#ifdef SimpleFS_dll_EXPORTS
#define SFS_EXPORT_API __declspec(dllexport)
#else
#define SFS_EXPORT_API
#endif

SFS_EXPORT_API simplefs_disk_handle_t *sfs_diskopen(const char *device, bool *need_format);

SFS_EXPORT_API void sfs_diskclose(simplefs_disk_handle_t *device);

SFS_EXPORT_API bool sfs_disk_create_empty(const char *disk, uint32_t disk_size);

SFS_EXPORT_API bool sfs_diskformat(simplefs_disk_handle_t *device, uint32_t disk_size, uint32_t block_size, const char *volume_name);

SFS_EXPORT_API uint32_t sfs_diskfree(simplefs_disk_handle_t *device);

SFS_EXPORT_API bool sfs_fcreate(simplefs_disk_handle_t *drive, const char *path);

SFS_EXPORT_API simplefs_file_t *sfs_fopen(simplefs_disk_handle_t *drive, const char *path, simplefs_open_mode_t mode);

SFS_EXPORT_API void sfs_fclose(simplefs_file_t *f);

SFS_EXPORT_API bool sfs_exists(simplefs_disk_handle_t *drive, const char *path);

SFS_EXPORT_API uint32_t sfs_fread(void *buffer, uint32_t size, simplefs_file_t *fp);

SFS_EXPORT_API uint32_t sfs_fwrite(const void *buffer, uint32_t size, simplefs_file_t *fp);

SFS_EXPORT_API bool sfs_fseek(simplefs_file_t *fp, int32_t offset, int seek_mode);

SFS_EXPORT_API void sfs_rewind(simplefs_file_t *fp);

SFS_EXPORT_API bool sfs_mkdir(simplefs_disk_handle_t *drive, const char *path);

SFS_EXPORT_API simplefs_dir_t *sfs_dir_open(simplefs_disk_handle_t *drive, const char *path);

SFS_EXPORT_API void sfs_dir_close(simplefs_dir_t *fp);

SFS_EXPORT_API bool sfs_tree_readdir(simplefs_dir_t *fp, simplefs_tree_read_result_t *result);

SFS_EXPORT_API void sfs_tree_rewind(simplefs_dir_t *fp);

SFS_EXPORT_API bool sfs_tree_rmdir(simplefs_disk_handle_t *drive, const char *path);

SFS_EXPORT_API bool sfs_remove(simplefs_disk_handle_t *drive, const char *path);

#endif
