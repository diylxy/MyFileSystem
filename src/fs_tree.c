#include <fs_tree.h>
#include <fs_block.h>
#include <fs_superblock.h>
#include <fs_general_file.h>
#include <fs_file.h>
#include <fs_tree.h>

FS_STATUS fs_tree_file_create(fs_handle_t *fs, fs_tree_handle_t *parent, const char *name, fs_tree_handle_t *result)
{
    uint32_t block_first = 0;
    TRUE_THEN_RETURN_FALSE(fs_general_file_create(fs->block, fs->superblock, name, FS_BLOCK_TREE_MAGIC, &block_first) == false); // 创建子目录文件
    TRUE_THEN_RETURN_FALSE(fs_general_file_seek(fs->block, parent, 0, SEEK_END) == false);                                       // 移动指针到父目录文件尾
    TRUE_THEN_RETURN_FALSE(fs_general_file_write(fs->block, fs->superblock, parent, &block_first, 4) != 4);                      // 写入子目录地址
    TRUE_THEN_RETURN_FALSE(fs_general_file_open(fs->block, fs->superblock, result, block_first) == false);                       // 打开子目录文件
    TRUE_THEN_RETURN_FALSE(fs_general_file_write(fs->block, fs->superblock, result, &(parent->block_first), 4) != 4);            // 写入父目录地址
    return true;
}

FS_STATUS fs_tree_readdir(fs_handle_t *fs, fs_tree_handle_t *tree, fs_tree_read_result_t *result)
{
    uint32_t next_block;
    TRUE_THEN_RETURN_FALSE(tree->header.magic != FS_BLOCK_TREE_MAGIC);
    if(tree->pos_current == tree->header.file_size)
        return false;
    TRUE_THEN_RETURN_FALSE(fs_general_file_read(fs->block, fs->superblock, tree, &next_block, 4) != 4);
    if (next_block == 0)
    {
        result->name[0] = '/';
        result->name[1] = '\0';
        result->is_dir = true;
        result->sub_file_handle = *tree;
        return true;
    }
    TRUE_THEN_RETURN_FALSE(fs_general_file_open(fs->block, fs->superblock, &result->sub_file_handle, next_block) == false);
    TRUE_THEN_RETURN_FALSE(fs_general_file_get_filename(fs->block, &result->sub_file_handle, result->name, 255) == false);
    if (result->sub_file_handle.header.magic == FS_BLOCK_TREE_MAGIC)
        result->is_dir = true;
    else if (result->sub_file_handle.header.magic == FS_BLOCK_FILE_MAGIC)
        result->is_dir = false;
    else
        return false;
    if (tree->pos_current == 4)
    {
        result->name[0] = '.';
        result->name[1] = '.';
        result->name[2] = '\0';
    }
    return true;
}
FS_STATUS fs_tree_append_entry(fs_handle_t *fs, fs_tree_handle_t *tree, uint32_t entry_blockid)
{
    TRUE_THEN_RETURN_FALSE(tree->header.magic != FS_BLOCK_TREE_MAGIC);
    TRUE_THEN_RETURN_FALSE(fs_general_file_seek(fs->block, tree, 0, SEEK_END) == false);
    TRUE_THEN_RETURN_FALSE(fs_general_file_write(fs->block, fs->superblock, tree, &entry_blockid, 4) != 4);
    TRUE_THEN_RETURN_FALSE(fs_general_file_sync(fs->block, tree) == false);
    return true;
}
FS_STATUS fs_tree_remove_entry(fs_handle_t *fs, fs_tree_handle_t *tree, uint32_t entry_blockid)
{
    uint32_t current_read_block = 0;
    uint32_t i;
    uint32_t entry_cnt;
    bool found_entry = false;
    TRUE_THEN_RETURN_FALSE(tree->header.magic != FS_BLOCK_TREE_MAGIC);
    TRUE_THEN_RETURN_FALSE(entry_blockid == 0);
    TRUE_THEN_RETURN_FALSE(entry_blockid == tree->block_first);
    entry_cnt = tree->header.file_size / 4 - 1;
    TRUE_THEN_RETURN_FALSE(entry_cnt == 0);
    TRUE_THEN_RETURN_FALSE(fs_general_file_seek(fs->block, tree, 0, SEEK_SET) == false);
    TRUE_THEN_RETURN_FALSE(fs_general_file_read(fs->block, fs->superblock, tree, &current_read_block, 4) != 4);
    for (i = 0; i < entry_cnt; ++i)
    {
        TRUE_THEN_RETURN_FALSE(fs_general_file_read(fs->block, fs->superblock, tree, &current_read_block, 4) != 4);
        if (current_read_block == entry_blockid)
        {
            uint32_t handle_snapshot_block_current;     // 当前读写的物理块号（准备覆盖的）
            uint32_t handle_snapshot_block_offset;      // 当前读写的块内偏移（准备覆盖的）
            uint32_t handle_snapshot_pos_current;       // 当前读写的位置(相对于文件第一个字节)（准备覆盖的）
            uint32_t handle_snapshot_end_block_current; // 当前读写的物理块号（最后一项）
            uint32_t handle_snapshot_end_block_offset;  // 当前读写的块内偏移（最后一项）
            uint32_t handle_snapshot_end_pos_current;   // 当前读写的位置(相对于文件第一个字节)（最后一项）
            found_entry = true;
            if (i == entry_cnt - 1)
            {
                TRUE_THEN_RETURN_FALSE(fs_general_file_seek(fs->block, tree, -4, SEEK_END) == false);
                break;
            }
            TRUE_THEN_RETURN_FALSE(fs_general_file_seek(fs->block, tree, -4, SEEK_CUR) == false);
            // 暂存准备覆盖的目录项的文件偏移量
            handle_snapshot_block_current = tree->block_current;
            handle_snapshot_block_offset = tree->block_offset;
            handle_snapshot_pos_current = tree->pos_current;
            TRUE_THEN_RETURN_FALSE(fs_general_file_seek(fs->block, tree, -4, SEEK_END) == false);
            // 暂存目录项最后一项起始文件偏移量
            handle_snapshot_end_block_current = tree->block_current;
            handle_snapshot_end_block_offset = tree->block_offset;
            handle_snapshot_end_pos_current = tree->pos_current;
            TRUE_THEN_RETURN_FALSE(fs_general_file_read(fs->block, fs->superblock, tree, &current_read_block, 4) != 4);
            // 快速移动至覆盖位置
            tree->block_current = handle_snapshot_block_current;
            tree->block_offset = handle_snapshot_block_offset;
            tree->pos_current = handle_snapshot_pos_current;
            TRUE_THEN_RETURN_FALSE(fs_general_file_write(fs->block, fs->superblock, tree, &current_read_block, 4) != 4);
            // 快速移动至文件尾部，准备缩小文件至文件指针
            tree->block_current = handle_snapshot_end_block_current;
            tree->block_offset = handle_snapshot_end_block_offset;
            tree->pos_current = handle_snapshot_end_pos_current;
            break;
        }
    }
    fs_general_file_trim_size_to_current_position(fs->block, fs->superblock, tree);
    return found_entry;
}

FS_STATUS fs_tree_rmdir(fs_handle_t *fs, fs_tree_handle_t *tree)
{
    TRUE_THEN_RETURN_FALSE(tree->header.magic != FS_BLOCK_TREE_MAGIC);
    TRUE_THEN_RETURN_FALSE(tree->header.file_size != 4);
    uint32_t parent;
    TRUE_THEN_RETURN_FALSE(fs_general_file_seek(fs->block, tree, 0, SEEK_SET) == false);
    TRUE_THEN_RETURN_FALSE(fs_general_file_read(fs->block, fs->superblock, tree, &parent, 4) != 4);
    fs_tree_handle_t parent_handle;
    TRUE_THEN_RETURN_FALSE(fs_general_file_open(fs->block, fs->superblock, &parent_handle, parent) == false);
    TRUE_THEN_RETURN_FALSE(fs_tree_remove_entry(fs, &parent_handle, tree->block_first) == false);
    TRUE_THEN_RETURN_FALSE(fs_general_file_close(fs->block, &parent_handle) == false);
    TRUE_THEN_RETURN_FALSE(fs_general_file_remove(fs->block, fs->superblock, tree->block_first) == false);
    return true;
}

bool fs_tree_exist(fs_handle_t *fs, fs_tree_handle_t *tree, const char *name) // 判断一个目录文件中是否存在某个文件名
{
    uint32_t next_block;
    fs_general_file_handle_t handle;
    char fn_buff[256];
    TRUE_THEN_RETURN_FALSE(fs_general_file_seek(fs->block, tree, 0, SEEK_SET) == false);
    TRUE_THEN_RETURN_FALSE(fs_general_file_read(fs->block, fs->superblock, tree, &next_block, 4) != 4);
    int i = tree->header.file_size / 4;
    while (--i)
    {
        TRUE_THEN_RETURN_FALSE(fs_general_file_read(fs->block, fs->superblock, tree, &next_block, 4) != 4);
        TRUE_THEN_RETURN_FALSE(fs_general_file_open(fs->block, fs->superblock, &handle, next_block) == false);
        TRUE_THEN_RETURN_FALSE(fs_general_file_get_filename(fs->block, &handle, fn_buff, 255) == false);
        if (strcmp(name, fn_buff) == 0)
        {
            return true;
        }
    }
    return false;
}

fs_tree_handle_t *fs_tree_open(fs_handle_t *fs, const char *path)
{
    fs_general_file_handle_t *fp;
    fp = fs_general_file_open_by_path(fs, NULL, path);
    if (fp != NULL)
    {
        if (fp->header.magic != FS_BLOCK_TREE_MAGIC)
        {
            free(fp);
            return NULL;
        }
        return fp;
    }
    return NULL;
}

void fs_tree_close(fs_handle_t *fs, fs_tree_handle_t *ptree)
{
    fs_general_file_close(fs->block, ptree);
    free(ptree);
}

FS_STATUS fs_tree_create(fs_handle_t *fs, const char *path)
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
    // 创建子目录
    fs_tree_handle_t subdir;
    if (fs_tree_file_create(fs, parent, new_tree_name, &subdir))
    {
        fs_general_file_close(fs->block, &subdir);
        fs_tree_close(fs, parent);
        return true;
    }
    fs_tree_close(fs, parent);
    return false;
}

fs_tree_handle_t *fs_tree_open_parent(fs_handle_t *fs, const char *path)
{
    fs_general_file_handle_t *fp;
    if (path[0] == '/')
        ++path;
    uint32_t path_len = strlen(path);
    if (path_len == 0)
        return fs_general_file_open_by_path(fs, NULL, "/");
    char *path_copy = (char *)malloc(path_len + 1);
    if(path_copy == NULL)return NULL;
    strcpy(path_copy, path);
    if(path_copy[path_len - 1] == '/')
    {
        path_copy[path_len - 1] = 0;
        path_len -= 1;
    }
    char *parent = strrchr(path_copy, '/');
    if(parent == NULL)
        return fs_general_file_open_by_path(fs, NULL, "/");
    *parent = 0;
    fp = fs_general_file_open_by_path(fs, NULL, path_copy);
    if (fp != NULL)
    {
        if (fp->header.magic != FS_BLOCK_TREE_MAGIC)
        {
            free(fp);
            fp = NULL;
        }
    }
    free(path_copy);
    return fp;
}

FS_STATUS fs_tree_rmdir_by_path(fs_handle_t *fs, const char *path)
{
    fs_tree_handle_t *tmp;
    tmp = fs_tree_open(fs, path);
    if(tmp == NULL) return false;
    TRUE_THEN_RETURN_FALSE(fs_tree_rmdir(fs, tmp) == false);
    free(tmp);
    return true;
}