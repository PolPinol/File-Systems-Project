#include <stdio.h>

#include "../include/ext2.h"
#include "../include/fat16.h"
#include "../include/phase3.h"

void phase3(const char *file_system, const char *file) {
  if (is_ext2(file_system)) {
    // PHASE 4: Not implemented
  } else if (is_fat16(file_system)) {
    cat_fat16(file_system, file);
  } else {
    printf("Unknown\n");
  }
}
