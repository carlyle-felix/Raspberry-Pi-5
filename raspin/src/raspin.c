#include <stdio.h>
#include <string.h>

#include "../include/gpiod-utils.h"
#include "../include/raspin-manager.h"

int main(int argc, char *argv[]) {
    Line_list rpi;

    if (argc == 1) {
        printf("idk what to do.\n");
        return 1;
    }

    rpi = populate_pins();
    if (!rpi) {
        return 1;
    }

    if (!strcmp(argv[1], "list")) {
        print_pins(rpi, PRINT_LIST);
    } else if (!strcmp(argv[1], "layout")) {
        printf("slow down, tiger.\n");
    }
    free_line_list(rpi);
    
    return 0;
}