#ifndef FAT16_H
#define FAT16_H

#include <stdint.h>

#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVE 0x20

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
  uint16_t creation_time;
  uint16_t creation_date;
  uint16_t last_access_date;
  uint16_t
      first_cluster_high;
  uint16_t last_write_time;
  uint16_t last_write_date;
  uint16_t first_cluster_low;
  uint32_t file_size;
} __attribute((packed)) fat16_dir_entry;

int is_fat16(const char *filename);
void metadata_fat16(const char *filename);
void tree_fat16(const char *file_system, int find_file, const char *file_name);
void cat_fat16(const char *file_system, const char *file);

#endif
