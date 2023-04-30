#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/fat16.h"
#include "../include/util.h"

fat16_boot_sector bs;

int is_fat16(const char *filename) {
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    printf("Error opening file\n");
    exit(EXIT_FAILURE);
  }

  fat16_boot_sector bs;
  fseek(fp, 0, SEEK_SET);
  fread(&bs, sizeof(fat16_boot_sector), 1, fp);

  if (strncmp(bs.fs_type, "FAT16", 5) != 0) {
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

void print_filename(uint8_t entry_filename[], int last, int depth,
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

void tree_fat16_subdir(FILE *fp, fat16_boot_sector bs, uint32_t current_sector,
                       int depth, int wasLast) {
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
      int last = FALSE;
      {
        uint16_t k;
        for (k = j + 1; k < (bs.bytes_per_sector / sizeof(fat16_dir_entry));
             k++) {
          fat16_dir_entry next_entry;
          fseek(fp,
                current_sector * bs.bytes_per_sector +
                    k * sizeof(fat16_dir_entry),
                SEEK_SET);
          fread(&next_entry, sizeof(fat16_dir_entry), 1, fp);

          if (next_entry.filename[0] != 0xE5 &&
              next_entry.filename[0] != 0x00) {
            break;
          }
        }

        if (k == (bs.bytes_per_sector / sizeof(fat16_dir_entry))) {
          last = TRUE;
        }
      }

      print_filename(entry.filename, last, depth, wasLast);

      uint32_t cluster =
          ((uint32_t)entry.first_cluster_high << 16) | entry.first_cluster_low;
      uint32_t data_region_start_sector =
          bs.reserved_sectors + (bs.number_of_fats * bs.sectors_per_fat) +
          root_directory_sectors;
      uint32_t new_sector =
          (cluster - 2) * bs.sectors_per_cluster + data_region_start_sector;

      tree_fat16_subdir(fp, bs, new_sector, depth + 1, last);
    } else if (entry.attributes == ATTR_ARCHIVE) {
      // see if it is the last entry in the directory
      int last = FALSE;
      {
        uint16_t k;
        for (k = j + 1; k < (bs.bytes_per_sector / sizeof(fat16_dir_entry));
             k++) {
          fat16_dir_entry next_entry;
          fseek(fp,
                current_sector * bs.bytes_per_sector +
                    k * sizeof(fat16_dir_entry),
                SEEK_SET);
          fread(&next_entry, sizeof(fat16_dir_entry), 1, fp);

          if (next_entry.filename[0] != 0xE5 &&
              next_entry.filename[0] != 0x00) {
            break;
          }
        }

        if (k == (bs.bytes_per_sector / sizeof(fat16_dir_entry))) {
          last = TRUE;
        }
      }

      print_filename(entry.filename, last, depth, wasLast);
    }
  }
}

void tree_fat16(const char *filename) {
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    printf("Error opening file\n");
    exit(EXIT_FAILURE);
  }

  printf(".\n");

  fat16_boot_sector bs;
  fseek(fp, 0, SEEK_SET);
  fread(&bs, sizeof(fat16_boot_sector), 1, fp);

  uint32_t first_root_sector =
      bs.reserved_sectors + (bs.number_of_fats * bs.sectors_per_fat);
  uint32_t root_directory_sectors =
      ((bs.root_dir_entries * 32) + (bs.bytes_per_sector - 1)) /
      bs.bytes_per_sector;
  for (uint32_t i = 0; i < root_directory_sectors; i++) {
    tree_fat16_subdir(fp, bs, first_root_sector + i, 0, FALSE);
  }
  fclose(fp);
}
