#ifndef EXT2_H
#define EXT2_H

#include <stdint.h>
#include <stdio.h>

// Constants for the ext2 file system
#define EXT2_SUPER_MAGIC 0xEF53
#define BASE_OFFSET 1024 

// Blocks are numbered starting from 1.
#define BLOCK_OFFSET(block) (BASE_OFFSET + (block - 1) * block_size)

// Constants relative to the data blocks
#define EXT2_NDIR_BLOCKS 12
#define EXT2_IND_BLOCK EXT2_NDIR_BLOCKS
#define EXT2_DIND_BLOCK (EXT2_IND_BLOCK + 1)
#define EXT2_TIND_BLOCK (EXT2_DIND_BLOCK + 1)
#define EXT2_N_BLOCKS (EXT2_TIND_BLOCK + 1)

// Ext2 directory file types.
#define EXT2_FT_REG_FILE 1
#define EXT2_FT_DIR 2

// Superblock structure
typedef struct {
  uint32_t s_inodes_count;      // Total number of inodes
  uint32_t s_blocks_count;      // Total number of blocks
  uint32_t s_r_blocks_count;    // Number of reserved blocks
  uint32_t s_free_blocks_count; // Number of free blocks
  uint32_t s_free_inodes_count; // Number of free inodes
  uint32_t s_first_data_block;  // First data block
  uint32_t s_log_block_size;    // Block size (log2)
  uint32_t unused1;
  uint32_t s_blocks_per_group;  // Number of blocks per group
  uint32_t unused2;   
  uint32_t s_inodes_per_group;  // Number of inodes per group
  uint32_t s_mtime;             // Mount time
  uint32_t s_wtime;             // Last write time
  uint32_t unused3;         
  uint16_t s_magic;             // Magic signature
  uint16_t unused4[3];
  uint32_t s_lastcheck;         // Time of last check
  uint32_t unused5[4];
  uint32_t s_first_ino;         // First non-reserved inode
  uint16_t s_inode_size;        // Size of inode structure
  uint16_t unused6;
  uint32_t s_feature_compat;    // Compatible feature set flags
  uint32_t unused7[6];         
  char s_volume_name[16];       // Volume name
} __attribute__((packed)) ext2_superblock;

// Group descriptor structure
typedef struct {
  uint32_t unused[2];
  uint32_t bg_inode_table;       // Inode table block
} __attribute__((packed)) ext2_group_desc;

// Inode structure
typedef struct {
  uint16_t i_mode;        // File mode
  uint16_t i_uid;         // Low 16 bits of owner uid
  uint32_t i_size;        // Size in bytes
  uint32_t i_atime;       // Access time
  uint32_t i_ctime;       // Creation time
  uint32_t i_mtime;       // Modification time
  uint32_t i_dtime;       // Deletion time
  uint16_t i_gid;         // Low 16 bits of group id
  uint16_t i_links_count; // Number of hard links
  uint32_t i_blocks;      // Blocks count IN DISK SECTORS
  uint32_t i_flags;       // File flags
  union {
    struct {
      uint32_t l_i_reserved1;
    } linux1;
    struct {
      uint32_t h_i_translator;
    } hurd1;
    struct {
      uint32_t m_i_reserved1;
    } masix1;
  } osd1;
  uint32_t i_block[15];   // Pointers to blocks
} __attribute__((packed)) ext2_inode;

typedef struct {
  uint32_t inode;           // Inode number
  uint16_t rec_len;         // Directory entry length
  uint8_t name_len;         // Name length
  uint8_t file_type;        // File type
  char name[255];           // File name
} __attribute__((packed)) ext2_dir_entry;

/**
* Function that checks if the file is an ext2 file
* @param filename: the name of the file
* @return TRUE if the file is an ext2 file, FALSE otherwise
*/
int is_ext2(const char *filename);

/**
* Function that prints the metadata of the ext2 file
* @param filename: the name of the file
*/
void metadata_ext2(const char *filename);

/**
* Function that prints the content of the ext2 file in a tree structure
* @param filename: the name of the file
*/
void tree_ext2(const char *filename);

#endif
