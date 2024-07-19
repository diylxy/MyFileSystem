#include <fs.h>
#define DISK_SIZE (1024 * 1024 * 4)
#define BLOCK_SIZE 1024
int main()
{
    fs_handle_t *fs;
    fs_general_file_handle_t root;
    bool need_format;
    fs_disk_create_empty("disk", DISK_SIZE);
    fs = fs_disk_open("disk", &need_format);
    if(need_format)
    {
        printf("Device need format.\n");
        fs_disk_format(fs, DISK_SIZE, BLOCK_SIZE, "SimpleFS");
    }
    printf("Disk created\n");
    // 打开根目录
    printf("打开根目录\n");
    fs_general_file_open(fs->block, fs->superblock, &root, fs->superblock->first_block);
    // 创建文件
    printf("创建文件\n");
    uint32_t first_block = 0;
    fs_general_file_create(fs->block, fs->superblock, "测试文件.txt", FS_BLOCK_FILE_MAGIC, &first_block);
    if(first_block > 0)
    {
        fs_tree_append_entry(fs, &root, first_block);
    }
    fs_general_file_close(fs->block, &root);
    // 遍历目录
    printf("遍历目录\n");
    fs_tree_read_result_t tree_walk_result;
    fs_general_file_open(fs->block, fs->superblock, &root, fs->superblock->first_block);
    while(fs_tree_readdir(fs, &root, &tree_walk_result))
    {
        if(tree_walk_result.is_dir)
        {
            printf("[TREE] ");
        }
        else
        {
            printf("[FILE] ");
        }
        printf("%11u ", tree_walk_result.sub_file_handle.header.file_size);
        printf("%11u ", tree_walk_result.sub_file_handle.header.create_time);
        printf("%11u ", tree_walk_result.sub_file_handle.header.modify_time);
        printf("/%s\n", tree_walk_result.name);
    }
    printf("关闭文件\n");
    fs_disk_close(fs);
    return 0;
}