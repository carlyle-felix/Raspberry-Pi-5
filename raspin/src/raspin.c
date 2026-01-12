#include <stdio.h>
#include <string.h>

#include "../include/raspin-manager.h"

#define CHIP "/dev/gpiochip0"

int main(int argc, char *argv[]) {
    Line_list rpi;

    if (argc == 1 || !strcmp(argv[1], "--help")) {
        printf("raspin:\tprint a list or layout of physical pins with corresponding gpio lines and fuctions.\n");
        printf("\n");
        printf("raspin [option]\n");
        printf("\n");
        printf("options:\n");
        printf("\tlist\t\tprint list of pins with corresponding gpio lines and funtions\n");
        printf("\tlayout\t\tprint layout of pins with corresponding gpio lines and funtions.\n");
        return 1;
    }

    rpi = populate_pins();
    if (!rpi) {
        return 1;
    }

    if (!strcmp(argv[1], "list")) {
        print_pins(CHIP, rpi, PRINT_LIST);
    } else if (!strcmp(argv[1], "layout")) {
        print_pins(CHIP, rpi, PRINT_LAYOUT);
    } else {
        printf("unkown option: %s, try --help\n", argv[1]);
    }
    free_line_list(rpi);
    
    return 0;
}