#include <fs_common.h>
#include <fs_general_file.h>
#include <fs_disk.h>
#include <fs_tree.h>
#include <fs_file.h>
#include <fs_api.h>

SFS_EXPORT_API simplefs_disk_handle_t *sfs_diskopen(const char *device, bool *need_format)
{
    return fs_disk_open(device, need_format);
}

SFS_EXPORT_API void sfs_diskclose(simplefs_disk_handle_t *device)
{
    fs_disk_close(device);
}

SFS_EXPORT_API bool sfs_disk_create_empty(const char *disk, uint32_t disk_size)
{
    return fs_disk_create_empty(disk, disk_size);
}

SFS_EXPORT_API bool sfs_diskformat(simplefs_disk_handle_t *device, uint32_t disk_size, uint32_t block_size, const char *volume_name)
{
    return fs_disk_format(device, disk_size, block_size, volume_name);
}

SFS_EXPORT_API uint32_t sfs_diskfree(simplefs_disk_handle_t *device)
{
    return fs_free_bitmap_get_free_block_total(device->block, device->superblock);
}

SFS_EXPORT_API bool sfs_fcreate(simplefs_disk_handle_t *drive, const char *path)
{
    return fs_file_create(drive, path);
}

SFS_EXPORT_API simplefs_file_t *sfs_fopen(simplefs_disk_handle_t *drive, const char *path, simplefs_open_mode_t mode)
{
    if (drive == NULL)
        return NULL;
    if (path == NULL)
        return NULL;
    simplefs_file_t *result = NULL;
    switch (mode)
    {
    case MODE_READ:
        result = (simplefs_file_t *)malloc(sizeof(simplefs_file_t));
        if (result == NULL)
            return NULL;
        result->fp = fs_file_open(drive, path);
        if (result->fp != NULL)
        {
            result->isopened = true;
            result->disk = drive;
            result->mode = MODE_READ;
        }
        else
        {
            free(result);
            result = NULL;
        }
        break;
    case MODE_WRITE:
        result = (simplefs_file_t *)malloc(sizeof(simplefs_file_t));
        if (result == NULL)
            return NULL;
        result->fp = fs_file_open(drive, path);
        if (result->fp != NULL)
        {
            result->isopened = true;
            result->disk = drive;
            result->mode = MODE_WRITE;
            fs_general_file_trim_size_to_current_position(drive->block, drive->superblock, result->fp);
        }
        else
        {
            if (fs_file_create(drive, path) == true && (result->fp = fs_file_open(drive, path)) != NULL)
            {
                result->isopened = true;
                result->disk = drive;
                result->mode = MODE_WRITE;
            }
            else
            {
                free(result);
                result = NULL;
            }
        }
        break;
    case MODE_APPEND:
        result = (simplefs_file_t *)malloc(sizeof(simplefs_file_t));
        if (result == NULL)
            return NULL;
        result->fp = fs_file_open(drive, path);
        if (result->fp != NULL)
        {
            result->isopened = true;
            result->disk = drive;
            result->mode = MODE_APPEND;
            fs_file_seek(drive, result->fp, 0, SEEK_END);
        }
        else
        {
            free(result);
            result = NULL;
        }
        break;
    default:
        return NULL;
        break;
    }
    return result;
}

SFS_EXPORT_API void sfs_fclose(simplefs_file_t *f)
{
    if (f == NULL)
        return;
    if (f->isopened == false)
        return;
    f->isopened = false;
    fs_file_close(f->disk, f->fp);
    f->disk = NULL;
    f->fp = NULL;
    free(f);
}

SFS_EXPORT_API bool sfs_exists(simplefs_disk_handle_t *drive, const char *path)
{
    simplefs_file_t *handle;
    handle = sfs_fopen(drive, path, MODE_READ);
    if (handle == NULL)
        return false;
    sfs_fclose(handle);
    return true;
}

SFS_EXPORT_API uint32_t sfs_fread(void *buffer, uint32_t size, simplefs_file_t *fp)
{
    if (fp == NULL)
        return false;
    if (fp->isopened == false)
        return false;
    return fs_general_file_read(fp->disk->block, fp->disk->superblock, fp->fp, buffer, size);
}

SFS_EXPORT_API uint32_t sfs_fwrite(const void *buffer, uint32_t size, simplefs_file_t *fp)
{
    if (fp == NULL)
        return false;
    if (fp->isopened == false)
        return false;
    if (fp->mode != MODE_WRITE && fp->mode != MODE_APPEND)
        return false;
    return fs_general_file_write(fp->disk->block, fp->disk->superblock, fp->fp, buffer, size);
}

SFS_EXPORT_API bool sfs_fseek(simplefs_file_t *fp, int32_t offset, int seek_mode)
{
    if (fp == NULL)
        return false;
    if (fp->isopened == false)
        return false;
    return fs_general_file_seek(fp->disk->block, fp->fp, offset, seek_mode);
}

SFS_EXPORT_API void sfs_rewind(simplefs_file_t *fp)
{
    fs_general_file_rewind(fp->disk->block, fp->fp);
}

SFS_EXPORT_API bool sfs_mkdir(simplefs_disk_handle_t *drive, const char *path)
{
    return fs_tree_create(drive, path);
}

SFS_EXPORT_API simplefs_dir_t *sfs_dir_open(simplefs_disk_handle_t *drive, const char *path)
{
    simplefs_dir_t *result = (simplefs_dir_t *)malloc(sizeof(simplefs_dir_t));
    if(result == NULL)
        return NULL;
    result->fp = fs_tree_open(drive, path);
    if(result->fp == NULL)
    {
        free(result);
        return NULL;
    }
    result->disk = drive;
    result->isopened = true;
    return result;
}

SFS_EXPORT_API void sfs_dir_close(simplefs_dir_t *fp)
{
    if (fp == NULL)
        return;
    if (fp->isopened == false)
        return;
    fp->isopened = false;
    fs_tree_close(fp->disk, fp->fp);
    fp->disk = NULL;
    fp->fp = NULL;
    free(fp);
}

SFS_EXPORT_API bool sfs_tree_readdir(simplefs_dir_t *fp, simplefs_tree_read_result_t *result)
{
    return fs_tree_readdir(fp->disk, fp->fp, result);
}

SFS_EXPORT_API void sfs_tree_rewind(simplefs_dir_t *fp)
{
    fs_general_file_rewind(fp->disk->block, fp->fp);
}

static bool rmdir_recursive_internal(simplefs_disk_handle_t *drive, uint32_t start_block)
{
    fs_general_file_handle_t handle;
    fs_general_file_open(drive->block, drive->superblock, &handle, start_block);
    if(handle.header.magic == FS_BLOCK_TREE_MAGIC)
    {
        uint32_t entry;
        fs_general_file_read(drive->block, drive->superblock, &handle, &entry, sizeof(uint32_t));
        while(fs_general_file_read(drive->block, drive->superblock, &handle, &entry, sizeof(uint32_t)) == sizeof(uint32_t))
        {
            if(entry == 0)
                continue;
            if(rmdir_recursive_internal(drive, entry) == false)
                return false;
        }
        fs_general_file_remove(drive->block, drive->superblock, start_block);
    }
    else if(handle.header.magic == FS_BLOCK_FILE_MAGIC)
    {
        fs_general_file_remove(drive->block, drive->superblock, start_block);
    }
    else
    {
        return false;
    }
    return true;
}
SFS_EXPORT_API bool sfs_tree_rmdir(simplefs_disk_handle_t *drive, const char *path)
{
    fs_tree_handle_t *handle = fs_tree_open(drive, path);
    if(handle == NULL)
        return false;
    uint32_t block = handle->block_first;
    uint32_t parent;
    fs_general_file_read(drive->block, drive->superblock, handle, &parent, sizeof(uint32_t));
    fs_tree_close(drive, handle);
    if(parent == 0)
        return false;
    if(rmdir_recursive_internal(drive, block))
    {
        fs_tree_handle_t parent_handle;
        if(fs_general_file_open(drive->block, drive->superblock, &parent_handle, parent) == false)
            return false;
        if(fs_tree_remove_entry(drive, &parent_handle, block) == false) return false;
        fs_general_file_close(drive->block, &parent_handle);
        return true;
    }
    return false;
}

SFS_EXPORT_API bool sfs_remove(simplefs_disk_handle_t *drive, const char *path)
{
    return fs_file_remove_by_path(drive, path);
}

