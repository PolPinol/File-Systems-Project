#ifndef FAT16_H
#define FAT16_H

#include <stdint.h>

#define FAT16_SECTOR_SIZE 512
#define FAT16_PT_SIZE 0x1BE

#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVE 0x20

#define DIR_FAT16 0
#define FILE_FAT16 1

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
void tree_fat16(const char *filename);

#endif
