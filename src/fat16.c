#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/fat16.h"
#include "../include/util.h"


int is_fat16(const char *filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        printf("Error opening file\n");
        exit(EXIT_FAILURE);
    }

    uint8_t buffer[FAT16_SECTOR_SIZE];
    fread(buffer, sizeof(buffer), 1, fp);

    // Create a pointer to the partition table within the buffer
    fat16_partition_table* partition_table = (fat16_partition_table*)&buffer[0x1BE];

    // Iterate over the partition table entries and print their properties
    fat16_partition_entry* partition_entry = NULL;
    for (int i = 0; i < 4; i++) {
        partition_entry = &partition_table->partition_table[i];
        if (partition_entry->start_sector_lba == 1) {
            break;
        }
    }

    if (partition_entry == NULL) {
        fclose(fp);
        return FALSE;
    }

    fat16_boot_sector* bs = (fat16_boot_sector*)&buffer[0];
    if (strncmp(bs->fs_type, "FAT16", 5) != 0) {
        fclose(fp);
        return FALSE;
    }

    fclose(fp);

    return TRUE;
}

void metadata_fat16(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printf("Error opening file\n");
        exit(EXIT_FAILURE);
    }

    uint8_t buffer[FAT16_SECTOR_SIZE];
    fread(buffer, sizeof(buffer), 1, fp);

    fat16_boot_sector* bs = (fat16_boot_sector*)&buffer[0];

    printf("\n------ Filesystem Information ------\n\n");
    printf("Filesystem: FAT16\n\n");

    printf("System name: %s\n", bs->oem);
    printf("Sector size: %d\n", bs->bytes_per_sector);
    printf("Sectors per cluster: %d\n", bs->sectors_per_cluster);
    printf("Reserved sectors: %d\n", bs->reserved_sectors);
    printf("# of FATs: %d\n", bs->number_of_fats);
    printf("Max root entries: %d\n", bs->root_dir_entries);
    printf("Sectors per FAT: %d\n", bs->sectors_per_fat);
    printf("Label: %.*s\n", 11, bs->volume_label);

    printf("\n");
    
    fclose(fp);
}

// TODO next phases
/*
// Create a pointer to the partition table within the buffer
fat16_partition_table* partition_table = (fat16_partition_table*)&buffer[0x1BE];

// Iterate over the partition table entries and print their properties
for (int i = 0; i < 4; i++) {
    fat16_partition_entry* partition_entry = &partition_table->partition_table[i];

    printf("Partition %d:\n", i+1);
    printf("Boot Indicator: 0x%02X\n", partition_entry->boot_indicator);
    printf("Start CHS: %d-%d-%d\n", partition_entry->start_cylinder, partition_entry->start_head, partition_entry->start_sector);
    printf("Partition Type: 0x%02X\n", partition_entry->partition_type);
    printf("End CHS: %d-%d-%d\n", partition_entry->end_cylinder, partition_entry->end_head, partition_entry->end_sector);
    printf("Start LBA: %d\n", partition_entry->start_sector_lba);
    printf("Total Sectors: %d\n", partition_entry->total_sectors);
}*/
