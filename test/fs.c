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
        printf("######################### Device needs format.\n");
        sfs_diskformat(fs, DISK_SIZE, BLOCK_SIZE, "SimpleFS");
    }
    // 创建文件
    printf("#########################Creating test1.bin\n");
    sfs_fcreate(fs, "/test1.bin");
    simplefs_file_t *fp = sfs_fopen(fs, "/测试1.bin", MODE_WRITE);
    sfs_fwrite("Hello World!\n123", 16, fp);
    sfs_fclose(fp);
    printf("#########################Creating testdir\n");
    sfs_mkdir(fs, "/testdir");
    // 遍历目录
    printf("######################### Walking root\n");
    simplefs_dir_t *root;
    root = sfs_dir_open(fs, "/");
    simplefs_tree_read_result_t tree_walk_result;
    printf("\n\n  Type      Block      大小    文件名\n");
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
        printf("%11u ", tree_walk_result.sub_file_handle.header.file_size);
        printf("%s\n", tree_walk_result.name);
    }
    printf("######################### Closing Root\n");
    sfs_dir_close(root);
    sfs_diskclose(fs);
    return 0;
}