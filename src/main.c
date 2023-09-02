#include <stdio.h>
#include <string.h>

#include "../include/phase1.h"
#include "../include/phase2.h"
#include "../include/phase3.h"

int main(int argc, char *argv[]) {
  // PHASE 1
  // ./fsutils --info <file system>

  // PHASE 2
  // ./fsutils --tree <file system>

  // PHASE 3
  // ./fsutils --cat <file system> <file>

  if (argc == 3) {
    if (strcmp(argv[1], "--info") == 0) {
      phase1(argv[2]);
    } else if (strcmp(argv[1], "--tree") == 0) {
      phase2(argv[2]);
    } else {
      printf("Error arguments\n");
    }
  } else if (argc == 4) {
    if (strcmp(argv[1], "--cat") == 0) {
      phase3(argv[2], argv[3]);
    } else {
      printf("Error arguments\n");
    }
  } else {
    printf("Error arguments\n");
  }

  return 0;
}
