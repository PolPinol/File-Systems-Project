#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/fat16.h"
#include "../include/util.h"

fat16_boot_sector bs;
fat16_dir_entry file_found;
int file_found_flag = FALSE;

int is_fat16(const char *filename) {

  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    printf("Error opening file\n");
    exit(EXIT_FAILURE);
  }

  fat16_boot_sector bs;
  fseek(fp, 0, SEEK_SET);
  fread(&bs, sizeof(fat16_boot_sector), 1, fp);


  if (bs.bytes_per_sector == 0) return FALSE;

  int root_dir_sectors = ((bs.root_dir_entries * 32) + (bs.bytes_per_sector - 1)) / bs.bytes_per_sector;
  int sectors_per_fat;
  uint32_t sectors_per_fat_32;
  int total_sectors;
  int data_sectors;
  int cluster_count;

  if (bs.sectors_per_fat != 0)
    sectors_per_fat = bs.sectors_per_fat;
  else {
    fseek(fp, 36, SEEK_SET);
    fread(&sectors_per_fat_32, sizeof(sectors_per_fat_32), 1, fp);
    sectors_per_fat = sectors_per_fat_32;
  }

  if(bs.total_sectors_small != 0)
    total_sectors = bs.total_sectors_small;
  else 
    total_sectors = bs.total_sectors_long;

  data_sectors = total_sectors - (bs.reserved_sectors + (bs.number_of_fats * sectors_per_fat) + root_dir_sectors);
  cluster_count = data_sectors / bs.sectors_per_cluster;
  
  fclose(fp);

  if (cluster_count >= 4085 && cluster_count < 65525) {
    return TRUE;
  }

  return FALSE;
}

void metadata_fat16(const char *filename) {
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    printf("Error opening file\n");
    exit(EXIT_FAILURE);
  }

  fat16_boot_sector bs;
  fseek(fp, 0, SEEK_SET);
  fread(&bs, sizeof(fat16_boot_sector), 1, fp);

  printf("\n------ Filesystem Information ------\n\n");
  printf("Filesystem: FAT16\n\n");

  printf("System name: %s\n", bs.oem);
  printf("Sector size: %d\n", bs.bytes_per_sector);
  printf("Sectors per cluster: %d\n", bs.sectors_per_cluster);
  printf("Reserved sectors: %d\n", bs.reserved_sectors);
  printf("# of FATs: %d\n", bs.number_of_fats);
  printf("Max root entries: %d\n", bs.root_dir_entries);
  printf("Sectors per FAT: %d\n", bs.sectors_per_fat);
  printf("Label: %.*s\n", 11, bs.volume_label);

  printf("\n");

  fclose(fp);
}

void _print_filename_tree(uint8_t entry_filename[], int last, int depth,
                    int wasLast) {
  char filename[15];
  int size_filename = 0;
  for (int k = 0; k < 8 && entry_filename[k] != 0x20; k++) {
    filename[size_filename] = tolower(entry_filename[k]);
    size_filename++;
  }

  for (int k = 8; k < 11 && entry_filename[k] != 0x20; k++) {
    if (k == 8) {
      filename[size_filename] = '.';
      size_filename++;
    }

    filename[size_filename] = tolower(entry_filename[k]);
    size_filename++;
  }

  filename[size_filename] = '\0';

  for (int i = 0; i < depth; i++) {
    if (i == 0 && depth != 1) {
      printf("│   ");
    } else if (wasLast) {
      printf("    ");
    } else {
      printf("│   ");
    }
  }
  if (last) {
    printf("└── %s\n", filename);
  } else {
    printf("├── %s\n", filename);
  }
}

int _is_last_entry(FILE *fp, uint32_t current_sector, int j, fat16_boot_sector bs) {
  int last = FALSE;
  {
    uint16_t k;
    for (k = j + 1; k < (bs.bytes_per_sector / sizeof(fat16_dir_entry)); k++) {
      fat16_dir_entry next_entry;
      fseek(fp,
            current_sector * bs.bytes_per_sector + k * sizeof(fat16_dir_entry),
            SEEK_SET);
      fread(&next_entry, sizeof(fat16_dir_entry), 1, fp);

      if (next_entry.filename[0] != 0xE5 && next_entry.filename[0] != 0x00) {
        break;
      }
    }

    if (k == (bs.bytes_per_sector / sizeof(fat16_dir_entry))) {
      last = TRUE;
    }
  }

  return last;
}

uint32_t _calculate_new_sector(fat16_dir_entry entry,
                               uint32_t root_directory_sectors, fat16_boot_sector bs) {
  uint32_t cluster =
      ((uint32_t)entry.first_cluster_high << 16) | entry.first_cluster_low;
  uint32_t data_region_start_sector = bs.reserved_sectors +
                                      (bs.number_of_fats * bs.sectors_per_fat) +
                                      root_directory_sectors;
  uint32_t new_sector =
      (cluster - 2) * bs.sectors_per_cluster + data_region_start_sector;

  return new_sector;
}

void _print_filename_cat(fat16_dir_entry entry, const char *file_name) {
  char filename[15];
  int size_filename = 0;
  for (int k = 0; k < 8 && entry.filename[k] != 0x20; k++) {
    filename[size_filename] = tolower(entry.filename[k]);
    size_filename++;
  }

  for (int k = 8; k < 11 && entry.filename[k] != 0x20; k++) {
    if (k == 8) {
      filename[size_filename] = '.';
      size_filename++;
    }

    filename[size_filename] = tolower(entry.filename[k]);
    size_filename++;
  }

  filename[size_filename] = '\0';

  if (strcmp(filename, file_name) == 0) {
    file_found = entry;
    file_found_flag = TRUE;
  }
}

void tree_fat16_subdir(FILE *fp, fat16_boot_sector bs, uint32_t current_sector,
                       int depth, int wasLast, int find_file,
                       const char *file_name) {
  uint32_t root_directory_sectors =
      ((bs.root_dir_entries * 32) + (bs.bytes_per_sector - 1)) /
      bs.bytes_per_sector;

  for (uint16_t j = 0; j < (bs.bytes_per_sector / sizeof(fat16_dir_entry));
       j++) {
    fat16_dir_entry entry;
    fseek(fp,
          current_sector * bs.bytes_per_sector + j * sizeof(fat16_dir_entry),
          SEEK_SET);
    fread(&entry, sizeof(fat16_dir_entry), 1, fp);

    // skip "." and ".."
    if (entry.filename[0] == 0x2E) {
      continue;
    }

    // skip illegal characters
    if (entry.filename[0] == 0xE5 || entry.filename[0] == 0x00) {
      continue;
    }

    if (entry.attributes == ATTR_DIRECTORY) {
      // see if it is the last entry in the directory
      int last = _is_last_entry(fp, current_sector, j, bs);

      if (!find_file) {
        _print_filename_tree(entry.filename, last, depth, wasLast);
      }

      tree_fat16_subdir(fp, bs,
                        _calculate_new_sector(entry, root_directory_sectors, bs),
                        depth + 1, last, find_file, file_name);
    } else if (entry.attributes == ATTR_ARCHIVE) {
      // see if it is the last entry in the directory
      int last = _is_last_entry(fp, current_sector, j, bs);

      if (find_file) {
        _print_filename_cat(entry, file_name);
      } else {
        _print_filename_tree(entry.filename, last, depth, wasLast);
      }
    }
  }
}

void tree_fat16(const char *file_system, int find_file, const char *file_name) {
  FILE *fp = fopen(file_system, "rb");
  if (!fp) {
    printf("Error opening file\n");
    exit(EXIT_FAILURE);
  }

  if (!find_file) {
    printf(".\n");
  }

  fat16_boot_sector bs;
  fseek(fp, 0, SEEK_SET);
  fread(&bs, sizeof(fat16_boot_sector), 1, fp);

  uint32_t first_root_sector =
      bs.reserved_sectors + (bs.number_of_fats * bs.sectors_per_fat);
  uint32_t root_directory_sectors =
      ((bs.root_dir_entries * 32) + (bs.bytes_per_sector - 1)) /
      bs.bytes_per_sector;
  for (uint32_t i = 0; i < root_directory_sectors; i++) {
    tree_fat16_subdir(fp, bs, first_root_sector + i, 0, FALSE, find_file,
                      file_name);
  }
  fclose(fp);
}

void cat_fat16(const char *file_system, const char *file) {
  file_found_flag = FALSE;

  tree_fat16(file_system, TRUE, file);

  if (file_found_flag == FALSE) {
    printf("File not found\n");
    exit(EXIT_FAILURE);
  } else {
    FILE *fp = fopen(file_system, "rb");
    if (!fp) {
      printf("Error opening file\n");
      exit(EXIT_FAILURE);
    }

    fat16_boot_sector bs;
    fseek(fp, 0, SEEK_SET);
    fread(&bs, sizeof(fat16_boot_sector), 1, fp);

    uint32_t root_directory_sectors =
        ((bs.root_dir_entries * 32) + (bs.bytes_per_sector - 1)) /
        bs.bytes_per_sector;
    uint32_t data_region_start_sector =
        bs.reserved_sectors + (bs.number_of_fats * bs.sectors_per_fat) +
        root_directory_sectors;
    uint32_t first_cluster = ((uint32_t)file_found.first_cluster_high << 16) |
                             file_found.first_cluster_low;
    uint32_t current_sector =
        (first_cluster - 2) * bs.sectors_per_cluster + data_region_start_sector;

    // read file from current_sector
    uint32_t file_size = file_found.file_size;
    uint32_t bytes_read = 0;

    while (bytes_read < file_size) {
      uint32_t bytes_to_read = bs.bytes_per_sector;
      if (bytes_read + bytes_to_read > file_size) {
        bytes_to_read = file_size - bytes_read;
      }

      char buffer[bytes_to_read];
      fseek(fp, current_sector * bs.bytes_per_sector, SEEK_SET);
      fread(buffer, bytes_to_read, 1, fp);

      for (uint32_t i = 0; i < bytes_to_read; i++) {
        printf("%c", buffer[i]);
      }

      bytes_read += bytes_to_read;
      current_sector++;
    }

    fclose(fp);
  }
}
