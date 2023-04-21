#ifndef EXT2_H
#define EXT2_H

#include <stdint.h>
#include <stdio.h>

// Constants for the ext2 file system
#define EXT2_SUPER_MAGIC 0xEF53
#define EXT2_SB_SIZE 1024 // Locates beginning of the super block (first group)
#define BASE_OFFSET 1024

// Blocks are numbered starting from 1.
#define BLOCK_OFFSET(block) (BASE_OFFSET + (block - 1) * block_size)

// Constants relative to the data blocks
#define EXT2_NDIR_BLOCKS 12
#define EXT2_IND_BLOCK EXT2_NDIR_BLOCKS
#define EXT2_DIND_BLOCK (EXT2_IND_BLOCK + 1)
#define EXT2_TIND_BLOCK (EXT2_DIND_BLOCK + 1)
#define EXT2_N_BLOCKS (EXT2_TIND_BLOCK + 1)

#define EXT2_NAME_LEN 255

// Ext2 directory file types.
enum {
  EXT2_FT_UNKNOWN,
  EXT2_FT_REG_FILE,
  EXT2_FT_DIR,
  EXT2_FT_CHRDEV,
  EXT2_FT_BLKDEV,
  EXT2_FT_FIFO,
  EXT2_FT_SOCK,
  EXT2_FT_SYMLINK,
  EXT2_FT_MAX
};

// Superblock structure
typedef struct {
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
} __attribute__((packed)) ext2_superblock;

// Group descriptor structure
typedef struct {
  uint32_t bg_block_bitmap;      // Block bitmap block
  uint32_t bg_inode_bitmap;      // Inode bitmap block
  uint32_t bg_inode_table;       // Inode table block
  uint16_t bg_free_blocks_count; // Number of free blocks in group
  uint16_t bg_free_inodes_count; // Number of free inodes in group
  uint16_t bg_used_dirs_count;   // Number of directories in group
  uint16_t bg_pad;
  uint32_t bg_reserved[3]; // Padding to the end of the structure
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
  uint32_t i_block[15];  // Pointers to blocks
  uint32_t i_generation; /* File version (for NFS) */
  uint32_t i_file_acl;   /* File ACL */
  uint32_t i_dir_acl;    /* Directory ACL */
  uint32_t i_faddr;      /* Fragment address */
  union {
    struct {
      uint8_t l_i_frag;  /* Fragment number */
      uint8_t l_i_fsize; /* Fragment size */
      uint16_t i_pad1;
      uint16_t l_i_uid_high; /* these 2 fields    */
      uint16_t l_i_gid_high; /* were reserved2[0] */
      uint32_t l_i_reserved2;
    } linux2;
    struct {
      uint8_t h_i_frag;  /* Fragment number */
      uint8_t h_i_fsize; /* Fragment size */
      uint16_t h_i_mode_high;
      uint16_t h_i_uid_high;
      uint16_t h_i_gid_high;
      uint32_t h_i_author;
    } hurd2;
    struct {
      uint8_t m_i_frag;  /* Fragment number */
      uint8_t m_i_fsize; /* Fragment size */
      uint16_t m_pad1;
      uint32_t m_i_reserved2[2];
    } masix2;
  } osd2;
} __attribute__((packed)) ext2_inode;

typedef struct {
  uint32_t inode;           // Inode number
  uint16_t rec_len;         // Directory entry length
  uint8_t name_len;         // Name length
  uint8_t file_type;        // File type
  char name[EXT2_NAME_LEN]; // File name
} __attribute__((packed)) ext2_dir_entry;

int is_ext2(const char *filename);
void metadata_ext2(const char *filename);
void tree_ext2(const char *filename);
void read_dir(FILE *fp, ext2_inode *inode, ext2_group_desc *group);
void read_dir_block(FILE *fp, uint32_t block_num, ext2_group_desc *group);

#endif
