#include <stdio.h>
#include <fs_api.h>

#define DISK_SIZE (1024 * 1024 * 4)
#define BLOCK_SIZE 1024
int main()
{
    simplefs_disk_handle_t *fs;
    bool need_format;
    sfs_disk_create_empty("disk", DISK_SIZE);
    fs = sfs_diskopen("disk", &need_format);
    if(need_format)
    {
        printf("设备需要格式化\n");
        sfs_diskformat(fs, DISK_SIZE, BLOCK_SIZE, "SimpleFS");
    }
    printf("虚拟块设备已打开\n");

    // 创建文件
    printf("创建文件\n");
    sfs_fcreate(fs, "/测试1.bin");
    simplefs_file_t *fp = sfs_fopen(fs, "/测试1.bin", MODE_WRITE);
    sfs_fwrite("Hello World!\n123", 16, fp);
    sfs_fclose(fp);
    printf("创建目录 /testdir\n");
    sfs_mkdir(fs, "/testdir");
    printf("创建目录 /testdir/123\n");
    sfs_mkdir(fs, "/testdir/123");
    printf("创建文件 /test2.bin\n");
    sfs_fcreate(fs, "/test2.bin");
    printf("创建文件 /testdir/123/test5.bin\n");
    sfs_fcreate(fs, "/testdir/123/test5.bin");
    // 删除目录
    sfs_remove(fs, "/testdir/12312/test5.bin");
    printf("删除文件 /testdir/123/test5.bin\n");
    sfs_remove(fs, "/testdir/123/test5.bin");
    printf("删除目录 /testdir/123\n");
    sfs_tree_rmdir(fs, "/testdir/123");
    // 遍历目录
    printf("遍历目录\n");
    simplefs_dir_t *root;
    root = sfs_dir_open(fs, "/");
    simplefs_tree_read_result_t tree_walk_result;
    printf("  类型      块号      创建时间    修改时间      大小    文件名\n");
    while(sfs_tree_readdir(root, &tree_walk_result))
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
    sfs_dir_close(root);
    sfs_diskclose(fs);
    return 0;
}