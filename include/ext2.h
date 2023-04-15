#ifndef EXT2_H
#define EXT2_H

#include <stdint.h>

// Constants for the ext2 file system
#define EXT2_SUPER_MAGIC 0xEF53
#define EXT2_BLOCK_SIZE 1024
#define EXT2_INODE_SIZE 128

// Directory entry structure
#define EXT2_NAME_LEN 255

// File types
#define EXT2_FT_UNKNOWN 0
#define EXT2_FT_REG_FILE 1
#define EXT2_FT_DIR 2
#define EXT2_FT_CHRDEV 3
#define EXT2_FT_BLKDEV 4
#define EXT2_FT_FIFO 5
#define EXT2_FT_SOCK 6
#define EXT2_FT_SYMLINK 7

// File system states
#define EXT2_VALID_FS 1
#define EXT2_ERROR_FS 2

// Error behavior
#define EXT2_ERRORS_CONTINUE 1
#define EXT2_ERRORS_RO 2
#define EXT2_ERRORS_PANIC 3

// Superblock structure
struct ext2_superblock {
  uint32_t s_inodes_count;      // Total number of inodes
  uint32_t s_blocks_count;      // Total number of blocks
  uint32_t s_r_blocks_count;    // Number of reserved blocks
  uint32_t s_free_blocks_count; // Number of free blocks
  uint32_t s_free_inodes_count; // Number of free inodes

  uint32_t s_first_data_block; // First data block
  uint32_t s_log_block_size;   // Block size (log2)
  uint32_t s_log_frag_size;    // Fragment size (log2)
  uint32_t s_blocks_per_group; // Number of blocks per group
  uint32_t s_frags_per_group;  // Fragments per group
  uint32_t s_inodes_per_group; // Number of inodes per group
  uint32_t s_mtime;            // Mount time
  uint32_t s_wtime;            // Last write time
  uint16_t s_mnt_count;        // Mount count
  uint16_t s_max_mnt_count;    // Maximal mount count
  uint16_t s_magic;            // Magic signature
  uint16_t s_state;            // File system state
  uint16_t s_errors;           // Behaviour when detecting errors
  uint16_t s_minor_rev_level;  // Minor revision level
  uint32_t s_lastcheck;        // Time of last check
  uint32_t s_checkinterval;    // Maximum time between checks
  uint32_t s_creator_os;       // Creator OS
  uint32_t s_rev_level;        // Revision level
  uint16_t s_def_resuid;       // Default uid for reserved blocks
  uint16_t s_def_resgid;       // Default gid for reserved blocks

  // The following fields are for EXT2_DYNAMIC_REV superblocks only.
  uint32_t s_first_ino;              // First non-reserved inode
  uint16_t s_inode_size;             // Size of inode structure
  uint16_t s_block_group_nr;         // Block group number of this superblock
  uint32_t s_feature_compat;         // Compatible feature set flags
  uint32_t s_feature_incompat;       // Incompatible feature set flags
  uint32_t s_feature_ro_compat;      // Readonly-compatible feature set flags
  uint8_t s_uuid[16];                // 128-bit uuid for volume
  char s_volume_name[16];            // Volume name
  char s_last_mounted[64];           // Directory where last mounted
  uint32_t s_algorithm_usage_bitmap; // For compression

  // Performance hints
  uint8_t s_prealloc_blocks;     // Number of blocks to try to preallocate
  uint8_t s_prealloc_dir_blocks; // Number to preallocate for directories
  uint16_t s_padding1;           // Padding to 32 bits

  // Journaling support
  uint8_t s_journal_uuid[16];    // UUID of journal superblock
  uint32_t s_journal_inum;       // Inode number of journal file
  uint32_t s_journal_dev;        // Device number of journal file
  uint32_t s_last_orphan;        // Start of list of inodes to delete
  uint32_t s_hash_seed[4];       // HTREE hash seed
  uint8_t s_def_hash_version;    // Default hash version to use
  uint8_t s_reserved_char_pad;   // Journal backup method
  uint16_t s_reserved_word_pad;  // Size of group descriptor
  uint32_t s_default_mount_opts; // Default mount options
  uint32_t s_first_meta_bg;      // First metablock block group
  uint32_t s_reserved[190];      // Padding to the end of the block
};

// Group descriptor structure
struct ext2_group_desc {
  uint32_t bg_block_bitmap;      // Block bitmap block
  uint32_t bg_inode_bitmap;      // Inode bitmap block
  uint32_t bg_inode_table;       // Inode table block
  uint16_t bg_free_blocks_count; // Number of free blocks in group
  uint16_t bg_free_inodes_count; // Number of free inodes in group
  uint16_t bg_used_dirs_count;   // Number of directories in group
  uint16_t bg_pad;
  uint32_t bg_reserved[3]; // Padding to the end of the structure
};

// Inode structure
struct ext2_inode {
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
  uint32_t osd1;          // OS dependent 1
  uint32_t i_block[15];   // Pointers to blocks
  uint32_t i_generation;  // File version (for NFS)
  uint32_t i_file_acl;    // File ACL
  uint32_t i_dir_acl;     // Directory ACL
  uint32_t i_faddr;       // Fragment address
  uint32_t extra[3];
};

struct ext2_dir_entry {
  uint32_t inode;           // Inode number
  uint16_t rec_len;         // Directory entry length
  uint8_t name_len;         // Name length
  uint8_t file_type;        // File type
  char name[EXT2_NAME_LEN]; // File name
};

int is_ext2(const char *filename);
void metadata_ext2(const char *filename);

#endif
