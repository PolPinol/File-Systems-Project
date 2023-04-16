#ifndef FAT16_H
#define FAT16_H

#include <stdint.h>

#define FAT16_SECTOR_SIZE 512
#define FAT16_PT_SIZE 0x1BE

typedef struct {
  uint8_t boot_indicator;
  uint8_t start_head;
  uint8_t start_sector;
  uint8_t start_cylinder;
  uint8_t partition_type;
  uint8_t end_head;
  uint8_t end_sector;
  uint8_t end_cylinder;
  uint32_t start_sector_lba;
  uint32_t total_sectors;
} __attribute((packed)) fat16_partition_entry;

typedef struct {
  uint8_t bootstrap_code[446];
  uint32_t disk_signature;
  uint16_t usually_zero;
  fat16_partition_entry partition_table[4];
  uint16_t boot_signature;
} __attribute__((packed)) fat16_partition_table;

typedef struct {
  uint8_t jmp[3];
  char oem[8];
  uint16_t bytes_per_sector;
  uint8_t sectors_per_cluster;
  uint16_t reserved_sectors;
  uint8_t number_of_fats;
  uint16_t root_dir_entries;
  uint16_t total_sectors_small;
  uint8_t media_descriptor;
  uint16_t sectors_per_fat;
  uint16_t sectors_per_track;
  uint16_t number_of_heads;
  uint32_t hidden_sectors;
  uint32_t total_sectors_long;
  uint8_t drive_number;
  uint8_t current_head;
  uint8_t boot_signature;
  uint32_t volume_id;
  char volume_label[11];
  char fs_type[8];
  uint8_t boot_code[448];
  uint16_t boot_sector_signature;
} __attribute((packed)) fat16_boot_sector;

typedef struct {
  uint8_t filename[11];
  uint8_t attributes;
  uint8_t reserved;
  uint8_t creation_time_tenths;
  uint16_t creation_time;    // Time file was created.
  uint16_t creation_date;    // Date file was created.
  uint16_t last_access_date; // Last access date.
  uint16_t
      first_cluster_high;     // High word of this entry's first cluster number.
  uint16_t last_write_time;   // Time of last write.
  uint16_t last_write_date;   // Date of last write.
  uint16_t first_cluster_low; // Low word of this entry's first cluster number.
  uint32_t file_size; // 32-bit unsigned holding this file's size in bytes.
} __attribute((packed)) fat16_dir_entry;

int is_fat16(const char *filename);
void metadata_fat16(const char *filename);

#endif
