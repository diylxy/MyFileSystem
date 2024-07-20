#include <fs_file.h>
#include <fs_block.h>
#include <fs_superblock.h>
#include <fs_general_file.h>
#include <fs_tree.h>

FS_STATUS fs_file_create(fs_handle_t *fs, const char *path)
{
    if (path[0] == '/')
    {
        ++path;
    }
    if (path[0] == 0)
    {
        return false;
    }
    uint32_t path_len = strlen(path);
    if (path[path_len - 1] == '/')
        return false;
    const char *new_tree_name = strrchr(path, '/');
    uint32_t new_tree_name_len;
    fs_tree_handle_t *parent;
    if (new_tree_name == NULL)
    {
        new_tree_name = path;
        new_tree_name_len = path_len;
        parent = fs_tree_open(fs, "/");
    }
    else
    {
        new_tree_name++;
        new_tree_name_len = strlen(new_tree_name);
        char *parent_path = (char *)malloc(path_len + 1);
        strncpy(parent_path, path, path_len - new_tree_name_len - 1);
        parent_path[path_len - new_tree_name_len - 1] = 0;
        parent = fs_tree_open(fs, parent_path);
        free(parent_path);
    }
    // 打开父目录
    if (parent == NULL)
        return false;
    // 判断是否存在
    if (fs_tree_exist(fs, parent, new_tree_name))
    {
        fs_tree_close(fs, parent);
        return false;
    }
    // 创建子文件
    uint32_t block_first = 0;
    TRUE_THEN_RETURN_FALSE(fs_general_file_create(fs->block, fs->superblock, new_tree_name, FS_BLOCK_FILE_MAGIC, &block_first) == false); // 创建子文件
    TRUE_THEN_RETURN_FALSE(fs_general_file_seek(fs->block, parent, 0, SEEK_END) == false);                                                // 移动指针到父目录文件尾
    TRUE_THEN_RETURN_FALSE(fs_general_file_write(fs->block, fs->superblock, parent, &block_first, 4) != 4);                               // 写入文件地址
    fs_tree_close(fs, parent);
    return true;
}

fs_general_file_handle_t *fs_general_file_open_by_path(fs_handle_t *fs, fs_tree_handle_t *parent, const char *path)
{
    int this_name_length = 0;
    const char *next_slash = NULL;
    bool parent_need_free = false;
    fs_tree_read_result_t *tree_walk_result = (fs_tree_read_result_t *)malloc(sizeof(fs_tree_read_result_t));
    fs_general_file_handle_t *fp = NULL;
    if (tree_walk_result == NULL)
        return NULL;

    if (parent == NULL)
    {
        parent = (fs_tree_handle_t *)malloc(sizeof(fs_tree_handle_t));
        fs_general_file_open(fs->block, fs->superblock, parent, fs->superblock->first_block);
        parent_need_free = true;
    }
    if (path[0] == '/')
        ++path;
    if (path[0] == 0)
    {
        free(tree_walk_result);
        return parent;
    }
    next_slash = strchr(path, '/');
    if (next_slash == NULL)
    {
        this_name_length = strlen(path);
    }
    else
    {
        this_name_length = next_slash - path;
    }

    // 遍历目录
    fs_general_file_rewind(fs->block, parent);
    while (fs_tree_readdir(fs, parent, tree_walk_result))
    {
        if (strncmp(path, tree_walk_result->name, this_name_length) == 0 && tree_walk_result->name[this_name_length] == 0) // 文件名匹配
        {
            if (next_slash == NULL || *(next_slash + 1) == 0) // 这是最后一级
            {
                fp = (fs_general_file_handle_t *)malloc(sizeof(fs_general_file_handle_t));
                memcpy(fp, &(tree_walk_result->sub_file_handle), sizeof(fs_general_file_handle_t));
            }
            else
            {
                fp = fs_general_file_open_by_path(fs, &(tree_walk_result->sub_file_handle), next_slash + 1);
            }
            break;
        }
    }

    if (parent_need_free)
    {
        free(parent);
    }
    free(tree_walk_result);
    return fp;
}

fs_file_handle_t *fs_file_open(fs_handle_t *fs, const char *path)
{
    fs_general_file_handle_t *fp;
    fp = fs_general_file_open_by_path(fs, NULL, path);
    if (fp != NULL)
    {
        if (fp->header.magic != FS_BLOCK_FILE_MAGIC)
        {
            free(fp);
            return NULL;
        }
        return fp;
    }
    return NULL;
}

uint32_t fs_file_write(fs_handle_t *fs, fs_file_handle_t *fp, void *buffer, uint32_t size)
{
    return fs_general_file_write(fs->block, fs->superblock, fp, buffer, size);
}

uint32_t fs_file_read(fs_handle_t *fs, fs_file_handle_t *fp, void *buffer, uint32_t size)
{
    return fs_general_file_read(fs->block, fs->superblock, fp, buffer, size);
}

FS_STATUS fs_file_seek(fs_handle_t *fs, fs_file_handle_t *fp, int32_t offset, int seek_method)
{
    return fs_general_file_seek(fs->block, fp, offset, seek_method);
}

void fs_file_close(fs_handle_t *fs, fs_file_handle_t *fp)
{
    fs_general_file_close(fs->block, fp);
    free(fp);
}

FS_STATUS fs_file_remove(fs_handle_t *fs, fs_tree_handle_t *parent, fs_file_handle_t *file)
{
    TRUE_THEN_RETURN_FALSE(file->header.magic != FS_BLOCK_FILE_MAGIC);
    TRUE_THEN_RETURN_FALSE(fs_tree_remove_entry(fs, parent, file->block_first) == false);
    TRUE_THEN_RETURN_FALSE(fs_general_file_sync(fs->block, parent) == false);
    TRUE_THEN_RETURN_FALSE(fs_general_file_remove(fs->block, fs->superblock, file->block_first) == false);
    return true;
}

FS_STATUS fs_file_remove_by_path(fs_handle_t *fs, const char *path)
{
    fs_file_handle_t *file = fs_file_open(fs, path);
    if (file == NULL)
        return false;
    if (file->header.magic != FS_BLOCK_FILE_MAGIC)
    {
        fs_file_close(fs, file);
        return false;
    }
    fs_tree_handle_t *parent = fs_tree_open_parent(fs, path);
    if (fs_tree_remove_entry(fs, parent, file->block_first) == false)
    {
        free(file);
        fs_tree_close(fs, parent);
        return false;
    }
    fs_tree_close(fs, parent);
    if (fs_general_file_remove(fs->block, fs->superblock, file->block_first) == false)
    {
        free(file);
        return false;
    }
    free(file);
    return true;
}
