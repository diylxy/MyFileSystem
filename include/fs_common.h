#ifndef __SIMPLEFS_FS_COMMON_H__
#define __SIMPLEFS_FS_COMMON_H__
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <fs_typedefs.h>

typedef bool FS_STATUS;

#define FS_SUPERBLOCK_HEADER 0x55AAAA00

#define FS_BLOCK_TREE_MAGIC 0xBAAD // just for fun
#define FS_BLOCK_FILE_MAGIC 0xF00D

#define TRUE_THEN_RETURN_FALSE(x) assert ((x) == false)

#endif