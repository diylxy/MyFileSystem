#include <fs_common.h>
#include <fs_general_file.h>
#include <fs_disk.h>
#include <fs_tree.h>
#include <fs_file.h>
#include <fs_api.h>

simplefs_disk_handle_t *sfs_diskopen(const char *device, bool *need_format)
{
    return fs_disk_open(device, need_format);
}

void sfs_diskclose(simplefs_disk_handle_t *device)
{
    fs_disk_close(device);
}

bool sfs_disk_create_empty(const char *disk, uint32_t disk_size)
{
    return fs_disk_create_empty(disk, disk_size);
}

bool sfs_diskformat(simplefs_disk_handle_t *device, uint32_t disk_size, uint32_t block_size, const char *volume_name)
{
    return fs_disk_format(device, disk_size, block_size, volume_name);
}

bool sfs_fcreate(simplefs_disk_handle_t *drive, const char *path)
{
    return fs_file_create(drive, path);
}

simplefs_file_t *sfs_fopen(simplefs_disk_handle_t *drive, const char *path, simplefs_open_mode_t mode)
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

void sfs_fclose(simplefs_file_t *f)
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

bool sfs_exists(simplefs_disk_handle_t *drive, const char *path)
{
    simplefs_file_t *handle;
    handle = sfs_fopen(drive, path, MODE_READ);
    if (handle == NULL)
        return false;
    sfs_fclose(handle);
    return true;
}

uint32_t sfs_fread(void *buffer, uint32_t size, simplefs_file_t *fp)
{
    if (fp == NULL)
        return false;
    if (fp->isopened == false)
        return false;
    return fs_general_file_read(fp->disk->block, fp->disk->superblock, fp->fp, buffer, size);
}

uint32_t sfs_fwrite(const void *buffer, uint32_t size, simplefs_file_t *fp)
{
    if (fp == NULL)
        return false;
    if (fp->isopened == false)
        return false;
    if (fp->mode != MODE_WRITE && fp->mode != MODE_APPEND)
        return false;
    return fs_general_file_write(fp->disk->block, fp->disk->superblock, fp->fp, buffer, size);
}

bool sfs_fseek(simplefs_file_t *fp, int32_t offset, int seek_mode)
{
    if (fp == NULL)
        return false;
    if (fp->isopened == false)
        return false;
    return fs_general_file_seek(fp->disk->block, fp->fp, offset, seek_mode);
}

void sfs_rewind(simplefs_file_t *fp)
{
    fs_general_file_rewind(fp->disk->block, fp->fp);
}

bool sfs_mkdir(simplefs_disk_handle_t *drive, const char *path)
{
    return fs_tree_create(drive, path);
}

simplefs_dir_t *sfs_dir_open(simplefs_disk_handle_t *drive, const char *path)
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

void sfs_dir_close(simplefs_dir_t *fp)
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

bool sfs_tree_readdir(simplefs_dir_t *fp, simplefs_tree_read_result_t *result)
{
    return fs_tree_readdir(fp->disk, fp->fp, result);
}

void sfs_tree_rewind(simplefs_dir_t *fp)
{
    fs_general_file_rewind(fp->disk->block, fp->fp);
}

bool sfs_tree_rmdir(simplefs_disk_handle_t *drive, const char *path)
{
    return fs_tree_rmdir_by_path(drive, path);
}

bool sfs_remove(simplefs_disk_handle_t *drive, const char *path)
{
    return fs_file_remove_by_path(drive, path);
}

