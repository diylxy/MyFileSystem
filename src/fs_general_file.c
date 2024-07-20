#include <fs_general_file.h>
#include <time.h>

FS_STATUS fs_general_file_open(fs_block_description_t *block, fs_superblock_t *superblock, fs_general_file_handle_t *handle, uint32_t blocknum)
{
    TRUE_THEN_RETURN_FALSE(fs_block_read(block, blocknum) == false);
    fs_general_file_block_header_t *block_header = (fs_general_file_block_header_t *)block->current_block_data;
    TRUE_THEN_RETURN_FALSE(block_header->block_previous != 0);
    fs_general_file_header_t *header = (fs_general_file_header_t *)(block->current_block_data + sizeof(fs_general_file_block_header_t));
    handle->header = *header;
    handle->block_first = blocknum;
    handle->block_current = blocknum;
    handle->block_offset = 0;
    handle->pos_current = 0;
    handle->changed = false;
    return true;
}

FS_STATUS fs_general_file_create(fs_block_description_t *block, fs_superblock_t *superblock, const char *file_name, uint16_t magic, uint32_t *block_first)
{
    uint16_t file_name_size = strlen(file_name);
    TRUE_THEN_RETURN_FALSE(file_name_size >= 256);
    uint32_t free_block = -1;
    TRUE_THEN_RETURN_FALSE(fs_free_bitmap_allocate(block, superblock, &free_block) == false);
    fs_general_file_block_header_t *block_header = (fs_general_file_block_header_t *)block->current_block_data;
    block_header->block_previous = 0;
    block_header->block_next = 0;
    fs_general_file_header_t *header = (fs_general_file_header_t *)(block->current_block_data + sizeof(fs_general_file_block_header_t));
    header->name_length = file_name_size;
    header->file_size = 0;
    header->create_time = time(NULL);
    header->modify_time = header->create_time;
    header->magic = magic;
    strcpy(block->current_block_data + sizeof(fs_general_file_block_header_t) + sizeof(fs_general_file_header_t), file_name);
    TRUE_THEN_RETURN_FALSE(fs_block_write(block, free_block) == false);
    *block_first = free_block;
    return true;
}

FS_STATUS fs_general_file_get_filename(fs_block_description_t *block, fs_general_file_handle_t *handle, char *buffer, uint32_t max_chr_count)
{
    TRUE_THEN_RETURN_FALSE(block == NULL);
    TRUE_THEN_RETURN_FALSE(handle == NULL);
    int file_name_len_to_read = handle->header.name_length;
    if (file_name_len_to_read > max_chr_count)
        file_name_len_to_read = max_chr_count;
    TRUE_THEN_RETURN_FALSE(fs_block_read(block, handle->block_first) == false);
    memcpy(buffer, block->current_block_data + sizeof(fs_general_file_block_header_t) + sizeof(fs_general_file_header_t), file_name_len_to_read);
    buffer[file_name_len_to_read] = 0;
    return true;
}

uint32_t fs_general_file_read(fs_block_description_t *block, fs_superblock_t *superblock, fs_general_file_handle_t *handle, void *buffer, uint32_t size)
{
    uint32_t read_size = 0;
    if (size > handle->header.file_size - handle->pos_current)
        size = handle->header.file_size - handle->pos_current;
    while (read_size < size)
    {
        uint32_t this_time_header_length;
        this_time_header_length = sizeof(fs_general_file_block_header_t);

        if (handle->block_current == handle->block_first) // 如果是第一个块
            this_time_header_length += sizeof(fs_general_file_header_t) + handle->header.name_length;

        uint32_t this_read_size = block->blocksize - this_time_header_length - handle->block_offset;
        if (this_read_size > size - read_size)
            this_read_size = size - read_size;

        if (fs_block_read(block, handle->block_current) == false)
            break;
        fs_general_file_block_header_t *block_header = (fs_general_file_block_header_t *)block->current_block_data;
        uint8_t *current_data;
        current_data = block->current_block_data + this_time_header_length + handle->block_offset;
        memcpy((uint8_t *)buffer + read_size, current_data, this_read_size);
        read_size += this_read_size;
        handle->pos_current += this_read_size;
        handle->block_offset += this_read_size;
        if (read_size < size)
        {
            if (block_header->block_next == 0)
                break;
            handle->block_current = block_header->block_next;
            handle->block_offset = 0;
        }
    }
    return read_size;
}

uint32_t fs_general_file_write(fs_block_description_t *block, fs_superblock_t *superblock, fs_general_file_handle_t *handle, const void *buffer, uint32_t size)
{
    uint32_t write_size = 0;
    uint32_t previous_block = 0;
    while (write_size < size)
    {
        uint32_t this_time_header_length;
        this_time_header_length = sizeof(fs_general_file_block_header_t);

        if (handle->block_current == handle->block_first) // 如果是第一个块
            this_time_header_length += sizeof(fs_general_file_header_t) + handle->header.name_length;

        uint32_t this_write_size = block->blocksize - this_time_header_length - handle->block_offset;
        if (this_write_size > size - write_size)
            this_write_size = size - write_size;

        if (previous_block == 0)
            if (fs_block_read(block, handle->block_current) == false) // 读取当前块，用于覆盖写入（或更新双向链表节点信息）
                break;
        fs_general_file_block_header_t *block_header = (fs_general_file_block_header_t *)block->current_block_data; // 获取块头信息用于更新链表
        uint8_t *current_data;                                                                                      // 当前要写入的位置
        current_data = block->current_block_data + this_time_header_length + handle->block_offset;
        memcpy(current_data, (const uint8_t *)buffer + write_size, this_write_size);
        write_size += this_write_size;
        handle->pos_current += this_write_size;
        handle->block_offset += this_write_size;
        if (previous_block != 0) // 这个块是新块
        {
            block_header->block_previous = previous_block; // 记录上一次循环的块地址
            block_header->block_next = 0;
        }

        if (write_size < size) // 如果超出文件已经申请的块数，但仍需写入
        {
            if (block_header->block_next == 0)
            {
                // 申请新块
                printf("Allocating new block\n");
                uint32_t free_block = -1;
                fs_block_write(block, handle->block_current); // 暂存当前数据区
                if (fs_free_bitmap_allocate(block, superblock, &free_block) == false)
                    break;
                fs_block_read(block, handle->block_current);
                block_header->block_next = free_block;
                previous_block = handle->block_current;
                if (fs_block_write(block, handle->block_current) == false)
                    break;
                handle->block_current = free_block;
                handle->block_offset = 0;
            }
            else
            {
                if (fs_block_write(block, handle->block_current) == false)
                    break;
                handle->block_current = block_header->block_next;
                handle->block_offset = 0;
                previous_block = 0;
            }
        }
        else
        {
            fs_block_write(block, handle->block_current);
        }
    }
    // 更新文件头
    if (handle->pos_current > handle->header.file_size)
    {
        handle->header.file_size = handle->pos_current;
    }
    if (write_size > 0)
        handle->changed = true;
    return write_size;
}

FS_STATUS fs_general_file_rewind(fs_block_description_t *block, fs_general_file_handle_t *handle)
{
    handle->block_current = handle->block_first;
    handle->block_offset = 0;
    handle->pos_current = 0;
    return true;
}

FS_STATUS fs_general_file_seek(fs_block_description_t *block, fs_general_file_handle_t *handle, int32_t offset, int seek_method)
{
    int64_t offset_actual;
    if (seek_method == SEEK_SET) // 从头开始
    {
        if (offset < 0 || offset > handle->header.file_size)
            return false;
        offset_actual = 0 - (int64_t)handle->pos_current;
        offset_actual += offset;
    }
    else if (seek_method == SEEK_END)
    {
        if (offset > 0 || offset < -(int32_t)(handle->header.file_size))
            return false;
        offset_actual = handle->header.file_size - handle->pos_current;
        offset_actual += offset;
    }
    else
    {
        uint32_t new_pos = offset + handle->pos_current;
        if (new_pos > handle->header.file_size || new_pos < 0)
            return false;
        offset_actual = offset;
    }
    if (offset_actual == 0)
        return true;
    fs_general_file_block_header_t *block_header;                                       // 用于处理文件数据链
    int32_t block_capacity = block->blocksize - sizeof(fs_general_file_block_header_t); // 暂存块容量，简化计算
    int32_t first_block_capacity = block_capacity - sizeof(fs_general_file_header_t) - handle->header.name_length;
    int64_t delta = handle->block_offset;
    delta += offset_actual;
    int64_t delta_block = delta / block_capacity;    // 需要移动的块数
    int32_t new_blk_offset = delta % block_capacity; // 新的块内偏移值
    if (new_blk_offset < 0)
    {
        delta_block -= 1;
        if (handle->pos_current + offset_actual < first_block_capacity)
            new_blk_offset += first_block_capacity;
        else
            new_blk_offset += block_capacity;
    }
    handle->block_offset = delta % block->blocksize;
    handle->pos_current += offset_actual;
    while (delta_block < 0)
    {
        fs_block_read(block, handle->block_current);
        block_header = (fs_general_file_block_header_t *)block->current_block_data;
        handle->block_current = block_header->block_previous;
        delta_block += 1;
    }
    while (delta_block > 0)
    {
        fs_block_read(block, handle->block_current);
        block_header = (fs_general_file_block_header_t *)block->current_block_data;
        handle->block_current = block_header->block_next;
        delta_block -= 1;
    }
    handle->block_offset = new_blk_offset;
    return true;
}

FS_STATUS fs_general_file_sync(fs_block_description_t *block, fs_general_file_handle_t *handle)
{
    if (handle->changed)
    {
        handle->changed = false;
        if (fs_block_read(block, handle->block_first) == false)
            return false;
        fs_general_file_header_t *file_header = (fs_general_file_header_t *)(block->current_block_data + sizeof(fs_general_file_block_header_t));
        handle->header.modify_time = time(NULL);
        *file_header = handle->header;
        if (fs_block_write(block, handle->block_first) == false)
            return false;
    }
    return true;
}

FS_STATUS fs_general_file_close(fs_block_description_t *block, fs_general_file_handle_t *handle)
{
    TRUE_THEN_RETURN_FALSE(fs_general_file_sync(block, handle) == false);
    return true;
}

FS_STATUS fs_general_file_remove(fs_block_description_t *block, fs_superblock_t *superblock, uint32_t blocknum)
{
    TRUE_THEN_RETURN_FALSE(fs_block_read(block, blocknum) == false);
    fs_general_file_block_header_t *header = (fs_general_file_block_header_t *)block->current_block_data;
    fs_general_file_header_t *header_file = (fs_general_file_header_t *)(block->current_block_data + sizeof(fs_general_file_block_header_t));
    memset(header_file, 0, sizeof(fs_general_file_header_t));
    fs_block_write(block, blocknum);
    while (blocknum != 0)
    {
        fs_free_bitmap_free(block, superblock, blocknum);
        blocknum = header->block_next;
        TRUE_THEN_RETURN_FALSE(fs_block_read(block, blocknum) == false);
    }
    return true;
}

FS_STATUS fs_general_file_trim_size_to_current_position(fs_block_description_t *block, fs_superblock_t *superblock, fs_general_file_handle_t *handle)
{
    fs_general_file_block_header_t *header = (fs_general_file_block_header_t *)block->current_block_data;
    uint32_t next_block; // 存储需要释放的块号
    TRUE_THEN_RETURN_FALSE(fs_block_read(block, handle->block_current) == false);
    next_block = header->block_next;
    while (next_block > 0)
    {
        fs_free_bitmap_free(block, superblock, next_block);
        TRUE_THEN_RETURN_FALSE(fs_block_read(block, next_block) == false);
        next_block = header->block_next;
    }
    if (handle->header.file_size != handle->pos_current)
    {
        handle->header.file_size = handle->pos_current;
        handle->changed = true;
    }
    return true;
}
