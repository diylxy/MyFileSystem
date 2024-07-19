#include <fs_block_tree.h>

typedef struct fs_block_tree_header_t
{
    uint32_t crc32;              // 当前块的CRC（固定）
    uint32_t magic;              // 目录（文件夹）识别头，4字节对齐
    uint32_t block_previous;    // 上一个块的块号（0表示这是该目录的第一个块）
    uint32_t block_next;        // 下一个块的块号（0表示这是该目录最后一个块）
    uint32_t block_parent;      // 父目录的块号（0表示这是根目录）
    // 之后每4字节一组，表示一个子目录或文件的块号，直到该块的末尾
} fs_block_tree_header_t;

