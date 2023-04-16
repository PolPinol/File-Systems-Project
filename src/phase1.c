#include <stdio.h>

#include "../include/phase1.h"
#include "../include/ext2.h"


void phase1(char *fileName) {
    if (is_ext2(fileName)) {
        metadata_ext2(fileName);
    } else {
        printf("Unknown\n");
    }
}
