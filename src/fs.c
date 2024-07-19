#include <fs.h>
#define DISK_SIZE (1024 * 1024 * 4)
#define BLOCK_SIZE 1024
int main()
{
    fs_block_description_t block;
    fs_superblock_t superblock;
    fs_handle_t fs;
    fs_disk_create_empty("disk", DISK_SIZE);
    fs_block_open(&block, "disk", BLOCK_SIZE);
    superblock.block_total = DISK_SIZE / BLOCK_SIZE;
    superblock.magic = FS_SUPERBLOCK_HEADER;
    superblock.version = 1;
    strcpy(superblock.volume_name, "SimpleFS");
    fs_superblock_update(&block, &superblock);
    fs_free_bitmap_format(&block, &superblock);
    // 创建根目录文件
    int tmp = 0;
    fs_general_file_handle_t root;
    fs_general_file_create(&block, &superblock, "/", FS_BLOCK_TREE_MAGIC, &superblock.first_block);
    fs_general_file_open(&block, &superblock, &root, superblock.first_block);
    fs_general_file_write(&block, &superblock, &root, &tmp, 4);
    fs_general_file_close(&block, &root);
    fs_superblock_update(&block, &superblock);
    printf("Disk created\n");
    fs.block = &block;
    fs.superblock = &superblock;
    // 打开根目录
    fs_general_file_open(fs.block, fs.superblock, &root, fs.superblock->first_block);
    // 创建文件
    uint32_t first_block = 0;
    fs_general_file_create(fs.block, fs.superblock, "测试文件.txt", FS_BLOCK_FILE_MAGIC, &first_block);
    if(first_block > 0)
    {
        fs_tree_append_entry(&fs, &root, first_block);
    }
    fs_general_file_close(fs.block, &root);
    // 遍历目录
    fs_tree_read_result_t tree_walk_result;
    fs_general_file_open(fs.block, fs.superblock, &root, fs.superblock->first_block);
    while(fs_tree_readdir(&fs, &root, &tree_walk_result))
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
    fs_block_close(&block);
    return 0;
}