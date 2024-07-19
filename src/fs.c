#include <fs.h>
#define DISK_SIZE (1024 * 1024 * 4)
#define BLOCK_SIZE 1024
int main()
{
    fs_block_description_t block;
    fs_superblock_t superblock;
    fs_disk_create_empty("disk", DISK_SIZE);
    fs_block_open(&block, "disk", BLOCK_SIZE);
    superblock.block_total = DISK_SIZE / BLOCK_SIZE;
    superblock.magic = FS_SUPERBLOCK_HEADER;
    superblock.version = 1;
    strcpy(superblock.volume_name, "SimpleFS");
    fs_superblock_update(&block, &superblock);
    fs_free_bitmap_format(&block, &superblock);
    superblock.first_block = fs_free_bitmap_get_total_block(&block, &superblock) + 1;
    fs_superblock_update(&block, &superblock);
    printf("Disk created\n");
    bool result;
    fs_general_file_handle_t hFile;
    uint32_t block_first;
    result = fs_general_file_create(&block, &superblock, "Hello world.txt", FS_BLOCK_TREE_MAGIC, &block_first);
    result = fs_general_file_open(&block, &superblock, &hFile, block_first);
    if (result)
        printf("block_first: %d\n", block_first);
    uint32_t size;
    for (int i = 0; i < 40; ++i)
    {
        size = fs_general_file_write(&block, &superblock, &hFile, "Hello12345""Hello12345""Hello12345""Hello12345""Hello12345""Hello12345""Hello12345""Hello12345""Hello12345""Hello12345", 100);
    }
    printf("write finished!\n");
    size = 3000;
    fs_general_file_seek(&block, &hFile, -size, SEEK_CUR);
    printf("seek finished!\n");
    char str[3000];
    printf("read begin!\n");
    size = fs_general_file_read(&block, &superblock, &hFile, str, 3000);
    if (size > 0)
    {
        printf("read size: %d\n", size);
        fwrite(str, size, 1, stdout);
        putchar('\n');
    }
    fs_general_file_close(&block, &hFile);
    fs_block_close(&block);
    return 0;
}