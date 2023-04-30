#include <stdio.h>

#include "../include/ext2.h"
#include "../include/fat16.h"
#include "../include/phase2.h"

void phase2(const char *fileName) {
  if (is_ext2(fileName)) {
    tree_ext2(fileName);
  } else if (is_fat16(fileName)) {
    tree_fat16(fileName);
  } else {
    printf("Unknown\n");
  }
}
