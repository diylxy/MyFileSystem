#include <stdio.h>
#include <fs_api.h>
#include <time.h>
void printTimeStamp(uint32_t timestamp)
{
    time_t t = timestamp;
    struct tm *tm = localtime(&t);
    printf("%d-%02d-%02d %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
}
#define DISK_SIZE (1024 * 1024 * 4)
#define BLOCK_SIZE 1024
int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: %s <disk> <directory>\n", argv[0]);
        return 0;
    }
    simplefs_disk_handle_t *fs;
    bool need_format;
    // sfs_disk_create_empty("disk", DISK_SIZE);
    // fs = sfs_diskopen("disk", &need_format);
    fs = sfs_diskopen(argv[1], &need_format);
    if(need_format)
    {
        printf("设备需要格式化\n");
        return 0;
        //sfs_diskformat(fs, DISK_SIZE, BLOCK_SIZE, "SimpleFS");
    }
    printf("虚拟块设备已打开\n");
    printf("可用空间：%d\n", sfs_diskfree(fs));
    printf("卷标：%s\n", fs->superblock->volume_name);
    // // 创建文件
    // printf("创建文件\n");
    // sfs_fcreate(fs, "/测试1.bin");
    // simplefs_file_t *fp = sfs_fopen(fs, "/测试1.bin", MODE_WRITE);
    // sfs_fwrite("Hello World!\n123", 16, fp);
    // sfs_fclose(fp);
    // printf("创建目录 /testdir\n");
    // sfs_mkdir(fs, "/testdir");
    // printf("创建目录 /testdir/123\n");
    // sfs_mkdir(fs, "/testdir/123");
    // printf("创建文件 /test2.bin\n");
    // sfs_fcreate(fs, "/test2.bin");
    // printf("创建文件 /testdir/123/test5.bin\n");
    // sfs_fcreate(fs, "/testdir/123/test5.bin");
    // // 删除目录
    // printf("删除目录 /testdir/123\n");
    // sfs_tree_rmdir(fs, "/testdir/123");
    // 遍历目录
    printf("遍历目录\n");
    simplefs_dir_t *root;
    root = sfs_dir_open(fs, argv[2]);
    simplefs_tree_read_result_t tree_walk_result;
    printf("  类型      块号               创建时间                   修改时间                 大小        文件名\n");
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
        printf("%11u        ", tree_walk_result.sub_file_handle.block_first);
        printTimeStamp(tree_walk_result.sub_file_handle.header.create_time);
        printf("          ");
        printTimeStamp(tree_walk_result.sub_file_handle.header.modify_time);
        printf("%11u        ", tree_walk_result.sub_file_handle.header.file_size);
        printf("%s\n", tree_walk_result.name);
    }
    printf("关闭文件\n");
    sfs_dir_close(root);
    printf("可用空间：%d块\n", sfs_diskfree(fs));
    sfs_diskclose(fs);
    return 0;
}