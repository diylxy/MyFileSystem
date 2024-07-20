#include <fs_block_free_bitmap.h>

typedef struct fs_block_free_bitmap_header_t
{
    uint32_t crc32; // 当前块的CRC（固定）
    // 之后直到该块的末尾，使用位示图的方式表示空闲块
} fs_block_free_bitmap_header_t;

static uint32_t get_free_bitmap_capacity(fs_block_description_t *block)
{
    return (block->blocksize - sizeof(fs_block_free_bitmap_header_t)) * 8;
}

uint32_t fs_free_bitmap_get_total_block(fs_block_description_t *block, fs_superblock_t *superblock) // 获取空闲块表所占空间
{
    int cap = get_free_bitmap_capacity(block);
    return superblock->block_total / cap + (superblock->block_total % cap == 0 ? 0 : 1);
}

FS_STATUS fs_free_bitmap_allocate(fs_block_description_t *block, fs_superblock_t *superblock, uint32_t *result) // 分配空闲块
{
    uint32_t total_block = fs_free_bitmap_get_total_block(block, superblock);
    uint32_t cap = get_free_bitmap_capacity(block);
    uint32_t fbmap_current = 1;

    for (fbmap_current = 1; fbmap_current < total_block + 1; fbmap_current++)
    {
        TRUE_THEN_RETURN_FALSE(fs_block_read(block, fbmap_current) == false);
        uint32_t first_not_ff = 0xffffffff; // 第一个非全FF的字节
        uint8_t bmp_first_no_ff;
        for (first_not_ff = 0; first_not_ff < block->blocksize - sizeof(fs_block_free_bitmap_header_t); first_not_ff++)
        {
            bmp_first_no_ff = block->current_block_data[sizeof(fs_block_free_bitmap_header_t) + first_not_ff];
            if (bmp_first_no_ff != 0xFF)
            {
                break;
            }
        }
        if (first_not_ff == 0xffffffff)
        {
            continue;
        }
        // 查找第一个0
        uint32_t first_zero_blocknum = 0;
        uint8_t i;
        for (i = 0; i < 8; i++)
        {
            if ((bmp_first_no_ff & (1 << i)) == 0)
            {
                first_zero_blocknum = i + first_not_ff * 8 + (fbmap_current - 1) * cap;
                if (first_zero_blocknum > superblock->block_total)
                {
                    printf("No free block\n");
                    return false; // 无可用空间
                }
                bmp_first_no_ff |= 1 << i;
                block->current_block_data[sizeof(fs_block_free_bitmap_header_t) + first_not_ff] = bmp_first_no_ff;
                fs_block_write(block, fbmap_current); // 记录该块已被使用
                *result = first_zero_blocknum;
                return true;
            }
        }
    }
    printf("No free block\n");
    return false;
}

FS_STATUS fs_free_bitmap_free(fs_block_description_t *block, fs_superblock_t *superblock, uint32_t blocknum) // 释放空闲块
{
    uint32_t total_block = fs_free_bitmap_get_total_block(block, superblock);
    uint32_t cap = get_free_bitmap_capacity(block);
    uint32_t fbmap_blkid = blocknum / cap + 1;
    uint32_t fbmap_offset = (blocknum % cap) / 8;
    uint32_t fbmap_bitoffset = (blocknum % cap) % 8;
    TRUE_THEN_RETURN_FALSE(fs_block_read(block, fbmap_blkid) == false);
    uint8_t bmp = block->current_block_data[sizeof(fs_block_free_bitmap_header_t) + fbmap_offset];
    bmp &= ~(1 << fbmap_bitoffset);
    block->current_block_data[sizeof(fs_block_free_bitmap_header_t) + fbmap_offset] = bmp;
    fs_block_write(block, fbmap_blkid);
    return true;
}

FS_STATUS fs_free_bitmap_format(fs_block_description_t *block, fs_superblock_t *superblock)
{
    uint32_t total_block = fs_free_bitmap_get_total_block(block, superblock);
    uint32_t byte_resv = (total_block + 1) / 8;
    uint32_t bit_resv = (total_block + 1) % 8;
    memset(block->current_block_data, 0, block->blocksize);
    for (uint32_t i = 1; i < total_block + 1; i++)
    {
        if (i == 1)
        {
            int j;
            for (j = 0; j < byte_resv; j++)
            {
                block->current_block_data[sizeof(fs_block_free_bitmap_header_t) + j] = 0xFF;
            }
            block->current_block_data[sizeof(fs_block_free_bitmap_header_t) + j] = (1 << bit_resv) - 1;
            printf("空闲块位示图: 文件系统头已占用%d个块\n", (1 << bit_resv) - 1);
        }
        fs_block_write(block, i);
    }
}

static const uint32_t countTable[256] = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};

uint32_t fs_free_bitmap_get_free_block_total(fs_block_description_t *block, fs_superblock_t *superblock)
{
    uint32_t total_block = fs_free_bitmap_get_total_block(block, superblock);
    uint32_t cap = get_free_bitmap_capacity(block);
    uint32_t fbmap_current = 1;
    uint32_t result = 0;
    for (fbmap_current = 1; fbmap_current < total_block + 1; fbmap_current++)
    {
        if (fs_block_read(block, fbmap_current) == false)
            return 0;
        uint32_t this_size = block->blocksize - sizeof(fs_block_free_bitmap_header_t);
        if(fbmap_current == total_block && this_size > (superblock->block_total / 8) % (block->blocksize - sizeof(fs_block_free_bitmap_header_t)))
            this_size = (superblock->block_total / 8) % (block->blocksize - sizeof(fs_block_free_bitmap_header_t));
        this_size += sizeof(fs_block_free_bitmap_header_t);
        for (uint32_t i = sizeof(fs_block_free_bitmap_header_t); i < this_size; ++i)
        {
            result += 8 - (countTable[block->current_block_data[i]]);
        }
    }
    return result;
}