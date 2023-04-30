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

/*
uint32_t FirstDataSector;
FirstDataSector = bs.reserved_sectors +
                (bs.number_of_fats * bs.sectors_per_fat) +
                root_directory_sectors;*/

void tree_fat16(const char *filename) {
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    printf("Error opening file\n");
    exit(EXIT_FAILURE);
  }

  fat16_boot_sector bs;
  fseek(fp, 0, SEEK_SET);
  fread(&bs, sizeof(fat16_boot_sector), 1, fp);

  uint32_t first_root_sector =
      bs.reserved_sectors + (bs.number_of_fats * bs.sectors_per_fat);
  uint32_t root_directory_sectors =
      ((bs.root_dir_entries * 32) + (bs.bytes_per_sector - 1)) /
      FAT16_SECTOR_SIZE;

  uint32_t current_sector = first_root_sector;
  for (uint32_t i = 0; i < root_directory_sectors; i++) {
    fseek(fp, current_sector * bs.bytes_per_sector, SEEK_SET);
    for (uint16_t j = 0; j < (bs.bytes_per_sector / sizeof(fat16_dir_entry));
         j++) {
      fat16_dir_entry entry;
      fread(&entry, sizeof(fat16_dir_entry), 1, fp);

      // long name - not supported
      if (entry.filename[6] == '~' && entry.filename[7] == '1') {
        continue;
      }

      // skip "." and ".."
      if (entry.filename[0] == 0x2E) {
        continue;
      }

      if (entry.attributes == ATTR_DIRECTORY) {
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
        printf("%s\n", filename);
      }
    }
    current_sector++;
  }

  fclose(fp);
}
