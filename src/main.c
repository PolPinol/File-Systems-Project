#include <stdio.h>
#include <string.h>

#include "../include/phase1.h"
#include "../include/phase2.h"
#include "../include/phase3.h"
#include "../include/phase4.h"

int main(int argc, char *argv[]) {
    // PHASE 1
    // ./fsutils --info <file system>

    // PHASE 2
    // ./fsutils --tree <file system>

    // PHASE 3
    // ./fsutils --cat libfat <file>

    // PHASE 4
    // ./fsutils --cat lolext <file>

    if (argc == 3) {
        if (strcmp(argv[1], "--info") == 0) {
            phase1();
        } else if (strcmp(argv[1], "--tree") == 0) {
            phase2();
        } else {
            printf("Error arguments");
        }
    } else if (argc == 4) {
        if (strcmp(argv[1], "--cat") == 0) {
            if (strcmp(argv[2], "libfat") == 0) {
                phase3();
            } else if (strcmp(argv[2], "lolext") == 0) {
                phase4();
            } else {
                printf("Error arguments");
            }
        } else {
            printf("Error arguments");
        }
    } else {
        printf("Error arguments");
    }

    return 0;
}
