#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/ext2.h"
#include "../include/util.h"


int is_ext2(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        printf("Error opening file\n");
        exit(EXIT_FAILURE);
    }

    struct ext2_superblock sb;
    fseek(fp, EXT2_BLOCK_SIZE, SEEK_SET);
    fread(&sb, sizeof(sb), 1, fp);

    // Check the magic number to verify it's an ext2 file system
    if (sb.s_magic != EXT2_SUPER_MAGIC) {
        fclose(fp);
        return FALSE;
    }

    fclose(fp);

    return TRUE;
}

void metadata_ext2(const char* filename) {
    // Open the file system file
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("Failed to open file system file");
        return;
    }

    // Read the superblock
    struct ext2_superblock sb;
    fseek(fp, EXT2_BLOCK_SIZE, SEEK_SET);
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
