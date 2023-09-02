#include <stdio.h>

#include "../include/phase1.h"
#include "../include/ext2.h"
#include "../include/fat16.h"


void phase1(const char *fileName) {
    if (is_ext2(fileName)) {
        metadata_ext2(fileName);
    } else if (is_fat16(fileName)) {
        metadata_fat16(fileName);
    } else {
        printf("Unknown\n");
    }
}
