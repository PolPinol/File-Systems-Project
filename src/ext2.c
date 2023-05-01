#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "../include/ext2.h"
#include "../include/util.h"

static uint16_t block_size = 0;
ext2_superblock sb;

int is_ext2(const char *filename) {
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    printf("Error opening file\n");
    exit(EXIT_FAILURE);
  }

  ext2_superblock sb;
  fseek(fp, EXT2_SB_SIZE, SEEK_SET);
  fread(&sb, sizeof(sb), 1, fp);

  // Check the magic number to verify it's an ext2 file system
  if (sb.s_magic != EXT2_SUPER_MAGIC) {
    fclose(fp);
    return FALSE;
  }

  fclose(fp);

  return TRUE;
}

void metadata_ext2(const char *filename) {
  // Open the file system file
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    perror("Failed to open file system file");
    return;
  }

  // Read the superblock
  ext2_superblock sb;
  fseek(fp, EXT2_SB_SIZE, SEEK_SET);
  fread(&sb, sizeof(sb), 1, fp);

  printf("\n------ Filesystem Information ------\n\n");
  printf("Filesystem: EXT2\n\n");
  printf("INODE INFO\n");
  printf("  Size: %d\n", sb.s_inode_size);
  printf("  Num Inodes: %d\n", sb.s_inodes_count);
  printf("  First Inode: %d\n", sb.s_first_ino);
  printf("  Inodes Group: %d\n", sb.s_inodes_per_group);
  printf("  Free Inodes: %d\n", sb.s_free_inodes_count);

  printf("\nINFO BLOCK\n");
  printf("  Block size: %d\n", 1024 << sb.s_log_block_size);
  printf("  Reserved blocks: %d\n", sb.s_r_blocks_count);
  printf("  Free blocks: %d\n", sb.s_free_blocks_count);
  printf("  Total blocks: %d\n", sb.s_blocks_count);
  printf("  First block: %d\n", sb.s_first_data_block);
  printf("  Group blocks: %d\n", sb.s_blocks_per_group);
  printf("  Group flags: %d\n", sb.s_feature_compat);

  printf("\nINFO VOLUME\n");
  printf("  Volume name: %s\n", sb.s_volume_name);
  printf("  Last Checked: %s\n", format_time(sb.s_lastcheck));
  printf("  Last Mounted: %s\n", format_time(sb.s_mtime));
  printf("  Last Written: %s\n", format_time(sb.s_wtime));

  printf("\n");

  fclose(fp);
}

void read_group_desc_ext2(FILE *fp, uint16_t block_group,
                          ext2_group_desc *group) {
  uint32_t offset = 2 + block_group * sb.s_blocks_per_group;
  fseek(fp, BLOCK_OFFSET(offset), SEEK_SET);
  fread(group, sizeof(ext2_group_desc), 1, fp);
}

void read_inode_ext2(FILE *fp, uint32_t inode_num, ext2_inode *inode) {
  uint32_t block_group = (inode_num - 1) / sb.s_inodes_per_group;
  uint32_t local_inode_index = (inode_num - 1) % sb.s_inodes_per_group;

  ext2_group_desc group;
  read_group_desc_ext2(fp, block_group, &group);

  uint32_t offset = group.bg_inode_table + block_group * sb.s_blocks_per_group;

  fseek(fp, BLOCK_OFFSET(offset) + (local_inode_index)*sb.s_inode_size,
        SEEK_SET);
  fread(inode, sizeof(ext2_inode), 1, fp);
}

void read_dir_block(FILE *fp, uint32_t block_num, ext2_group_desc *group,
                    int depth, int imLast) {
  if (block_num == 0) {
    // End of blocks
    return;
  }

  // Read the block data
  uint8_t block[block_size];
  fseek(fp, BLOCK_OFFSET(block_num), SEEK_SET);
  fread(block, block_size, 1, fp);

  // Iterate over the directory entries within the block
  uint8_t *p = block;
  while (p < block + block_size) {
    ext2_dir_entry *entry = (ext2_dir_entry *)p;
    if (entry->inode == 0) {
      // End of entries
      break;
    }

    // Print the name of the directory entry
    char name[entry->name_len + 1];
    memcpy(name, entry->name, entry->name_len);
    name[entry->name_len] = '\0';

    // Check if it is a directory
    if ((entry->file_type & EXT2_FT_DIR) == EXT2_FT_DIR) {
      if (strcmp(name, ".") != 0 && strcmp(name, "..") != 0) {
        for (int i = 0; i < depth; i++) {
          if (i == 0) {
            printf("│");
          } else {
            printf(" ");
          }
          printf("   ");
        }
        if ((p + entry->rec_len) >= block + block_size) {
          printf("└── %s\n", name);
          imLast = TRUE;
        } else {
          printf("├── %s\n", name);
        }

        // Recursively traverse the directory
        ext2_inode inode;
        read_inode_ext2(fp, entry->inode, &inode);

        read_dir(fp, &inode, group, depth + 1, imLast);
      }
    } else if ((entry->file_type & EXT2_FT_REG_FILE) == EXT2_FT_REG_FILE) {
      // Print the name of the file
      for (int i = 0; i < depth; i++) {
        if (i == 0 && depth != 1) {
          printf("│   ");
        } else if (imLast) {
          printf("    ");
        } else {
          printf("│   ");
        }
      }
      if ((p + entry->rec_len) >= block + block_size) {
        printf("└── %s\n", name);
      } else {
        printf("├── %s\n", name);
      }
    }

    // Move to the next directory entry
    p += entry->rec_len;
  }
}

void read_dir(FILE *fp, ext2_inode *inode, ext2_group_desc *group, int depth,
              int imLast) {
  if (S_ISDIR(inode->i_mode)) {
    for (int i = 0; i < EXT2_N_BLOCKS; i++) {
      if (inode->i_block[i] == 0) {
        break;
      }

      if (i < EXT2_NDIR_BLOCKS) {
        read_dir_block(fp, inode->i_block[i], group, depth, imLast);
      } else if (i == EXT2_IND_BLOCK) {
        // Read directory entries from single indirect block
        uint32_t *ind_block = (uint32_t *)malloc(block_size);
        fseek(fp, BLOCK_OFFSET(inode->i_block[i]), SEEK_SET);
        fread(ind_block, block_size, 1, fp);

        for (int j = 0; j < (int)(block_size / sizeof(uint32_t)); j++) {
          if (ind_block[j] != 0) {
            read_dir_block(fp, ind_block[j], group, depth, imLast);
          }
        }
        free(ind_block);
      } else if (i == EXT2_DIND_BLOCK) {
        // Read directory entries from double indirect block
        uint32_t *ind_block = (uint32_t *)malloc(block_size);
        fseek(fp, BLOCK_OFFSET(inode->i_block[i]), SEEK_SET);
        fread(ind_block, block_size, 1, fp);

        for (int j = 0; j < (int)(block_size / sizeof(uint32_t)); j++) {
          if (ind_block[j] != 0) {
            uint32_t *dbl_ind_block = (uint32_t *)malloc(block_size);
            fseek(fp, BLOCK_OFFSET(ind_block[j]), SEEK_SET);
            fread(dbl_ind_block, block_size, 1, fp);

            for (int k = 0; k < (int)(block_size / sizeof(uint32_t)); k++) {
              if (dbl_ind_block[k] != 0) {
                read_dir_block(fp, dbl_ind_block[k], group, depth, imLast);
              }
            }

            free(dbl_ind_block);
          }
        }
        free(ind_block);
      } else if (i == EXT2_TIND_BLOCK) {
        // Read directory entries from triple indirect block
        uint32_t *ind_block = (uint32_t *)malloc(block_size);
        fseek(fp, BLOCK_OFFSET(inode->i_block[i]), SEEK_SET);
        fread(ind_block, block_size, 1, fp);

        for (int j = 0; j < (int)(block_size / sizeof(uint32_t)); j++) {
          if (ind_block[j] != 0) {
            uint32_t *dbl_ind_block = (uint32_t *)malloc(block_size);
            fseek(fp, BLOCK_OFFSET(ind_block[j]), SEEK_SET);
            fread(dbl_ind_block, block_size, 1, fp);

            for (int k = 0; k < (int)(block_size / sizeof(uint32_t)); k++) {
              if (dbl_ind_block[k] != 0) {
                uint32_t *trp_ind_block = (uint32_t *)malloc(block_size);
                fseek(fp, BLOCK_OFFSET(dbl_ind_block[k]), SEEK_SET);
                fread(trp_ind_block, block_size, 1, fp);

                for (int l = 0; l < (int)(block_size / sizeof(uint32_t)); l++) {
                  if (trp_ind_block[l] != 0) {
                    read_dir_block(fp, trp_ind_block[l], group, depth, imLast);
                  }
                }

                free(trp_ind_block);
              }
            }

            free(dbl_ind_block);
          }
        }
        free(ind_block);
      }
    }
  }
}

void tree_ext2(const char *filename) {
  // Open the file system file
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    perror("Failed to open file system file");
    return;
  }

  printf(".\n");

  // Read the superblock (first block group)
  fseek(fp, BASE_OFFSET, SEEK_SET);
  fread(&sb, sizeof(sb), 1, fp);

  // Block group size in bytes
  block_size = 1024 << sb.s_log_block_size;

  // Read first group descriptor from Block Group 0
  uint16_t block_group = 0;
  ext2_group_desc group;
  read_group_desc_ext2(fp, block_group, &group);

  // inodes per block group
  uint16_t inodes_per_block = block_size / sb.s_inode_size;

  for (uint32_t inode_num = 1; inode_num <= inodes_per_block; inode_num++) {
    ext2_inode inode;
    read_inode_ext2(fp, inode_num, &inode);

    read_dir(fp, &inode, &group, 0, FALSE);
  }

  printf("\n");

  fclose(fp);
}
