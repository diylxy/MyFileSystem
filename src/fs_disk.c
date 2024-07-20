#include <fs_block.h>
#include <fs_superblock.h>
#include <fs_general_file.h>
#include <fs_tree.h>
#include <fs_disk.h>

FS_STATUS fs_disk_create_empty(const char *file_name, uint32_t size_in_bytes)
{
    FILE *fp = fopen(file_name, "w");
    TRUE_THEN_RETURN_FALSE(fp == NULL);
    uint8_t block[512];
    memset(block, 0, sizeof(block));
    for (uint32_t i = 0; i < size_in_bytes / sizeof(block); i++)
    {
        fwrite(block, sizeof(block), 1, fp);
    }
    fclose(fp);
    return true;
}

FS_STATUS fs_disk_format(fs_handle_t *fs, uint32_t disk_size, uint32_t block_size, const char *volume_name)
{
    fs->superblock->block_total = disk_size / block_size;
    fs->superblock->magic = FS_SUPERBLOCK_HEADER;
    fs->superblock->version = SIMPLE_FS_VERSION;
    fs->superblock->block_size = block_size;
    fs_block_reset_blocksize(fs->block, block_size);
    strcpy(fs->superblock->volume_name, volume_name);
    fs_superblock_update(fs->block, fs->superblock);
    fs_free_bitmap_format(fs->block, fs->superblock);
    // 创建根目录文件
    int tmp = 0;
    fs_general_file_handle_t root;
    fs_general_file_create(fs->block, fs->superblock, "/", FS_BLOCK_TREE_MAGIC, &(fs->superblock->first_block));
    fs_general_file_open(fs->block, fs->superblock, &root, fs->superblock->first_block);
    fs_general_file_write(fs->block, fs->superblock, &root, &tmp, 4);
    fs_general_file_close(fs->block, &root);
    fs_superblock_update(fs->block, fs->superblock);
    return true;
}

fs_handle_t *fs_disk_open(const char *device, bool *need_format)
{
    fs_handle_t *fs;
    TRUE_THEN_RETURN_FALSE(device == NULL);
    TRUE_THEN_RETURN_FALSE(need_format == NULL);
    fs = (fs_handle_t *)malloc(sizeof(fs_handle_t));
    if (fs == NULL)
        return NULL;

    fs->block = (fs_block_description_t *)malloc(sizeof(fs_block_description_t));
    if (fs->block == NULL)
    {
        free(fs);
        return NULL;
    }
    fs->superblock = (fs_superblock_t *)malloc(sizeof(fs_superblock_t));
    if (fs->superblock == NULL)
    {
        free(fs);
        free(fs->block);
        return NULL;
    }
    fs_block_open(fs->block, "disk", 512);
    *need_format = fs_superblock_getinfo(fs->block, fs->superblock) == false;
    if (*need_format == false)
    {
        if (fs_block_reset_blocksize(fs->block, fs->superblock->block_size) == false) // 校验失败
        {
            *need_format = true;
        }
    }
    return fs;
}

void fs_disk_close(fs_handle_t *device_handle)
{
    if(device_handle == NULL) return;
    fs_block_close(device_handle->block);
    free(device_handle->block);
    free(device_handle->superblock);
    free(device_handle);
}
