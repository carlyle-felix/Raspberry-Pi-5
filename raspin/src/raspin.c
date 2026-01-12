#include <stdio.h>

#include "../include/gpiod-utils.h"
#include "../include/raspin-manager.h"

int main(int argc, char *argv[]) {
    Line_list rpi;

    rpi = populate_pins();
    if (!rpi) {
        return 1;
    }
    print_pins(rpi, PRINT_LIST);
    free_line_list(rpi);
    
    return 0;
}