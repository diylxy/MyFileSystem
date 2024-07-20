#include <fs.h>
#define DISK_SIZE (1024 * 1024 * 4)
#define BLOCK_SIZE 1024
int main()
{
    fs_handle_t *fs;
    bool need_format;
    fs_disk_create_empty("disk", DISK_SIZE);
    fs = fs_disk_open("disk", &need_format);
    if(need_format)
    {
        printf("设备需要格式化\n");
        fs_disk_format(fs, DISK_SIZE, BLOCK_SIZE, "SimpleFS");
    }
    printf("虚拟块设备已打开\n");

    // 创建文件
    printf("创建文件\n");
    fs_file_create(fs, "测试1.bin");
    fs_file_handle_t *fp = fs_file_open(fs, "/测试1.bin");
    fs_general_file_write(fs->block, fs->superblock, fp, "Hello World!\n123", 16);
    fs_file_close(fs, fp);
    fs_tree_create(fs, "/testdir");
    fs_tree_create(fs, "/testdir/123");
    fs_tree_create(fs, "/testdir/12312");
    fs_file_create(fs, "/test2.bin");
    fs_file_create(fs, "/测试3.bin");
    fs_file_create(fs, "/测试4.bin");
    fs_file_create(fs, "/测试5.bin");
    fs_file_create(fs, "/测试6.bin");
    fs_file_create(fs, "/testdir/12312/测试5.bin");
    // 遍历目录
    printf("遍历目录\n");
    fs_tree_handle_t *root;
    root = fs_tree_open(fs, "/");
    fs_tree_read_result_t tree_walk_result;
    printf("  类型      块号      创建时间    修改时间      大小    文件名\n");
    while(fs_tree_readdir(fs, root, &tree_walk_result))
    {
        if(tree_walk_result.is_dir)
        {
            printf("[TREE] ");
        }
        else
        {
            printf("[FILE] ");
        }
        printf("%11u ", tree_walk_result.sub_file_handle.block_first);
        printf("%11u ", tree_walk_result.sub_file_handle.header.create_time);
        printf("%11u ", tree_walk_result.sub_file_handle.header.modify_time);
        printf("%11u ", tree_walk_result.sub_file_handle.header.file_size);
        printf("%s\n", tree_walk_result.name);
    }
    printf("关闭文件\n");
    fs_tree_close(fs, root);
    fs_disk_close(fs);
    return 0;
}