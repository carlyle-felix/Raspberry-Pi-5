#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "../include/rpins.h"

enum print {PRINT_LIST, PRINT_LAYOUT};
struct line {
    uint8_t pin;
    int8_t gpio;
    char *line_fx;
    char *consumer;
    struct line *next;
};

#define NUM_PINS 40
const int8_t gpio_list[NUM_PINS] = {-1, -1, 2, -1, 3, -1, 4, 14, -1, 15, \
                                    17, 18, 27, -1, 22, 23, -1, 24, 10, -1, \
                                    9, 25, 11, 8, -1, 7, 0, 1, 5, -1, \
                                    6, 12, 13, -1, 19, 16, 26, 20, -1, 21};
const char *fx_list[] = {[0] = "3.3V", [1] = "5V", [2] = "SDA", [3] = "5V", [4] = "SCL", [5] = "GND", \
                        [6] = "GPCLK0", [7] = "TXD", [8] = "GND", [9] = "RXD", [11] = "PCM_CLK", [13] = "GND", \
                        [16] = "3.3V", [18] = "MOSI", [19] = "GND", [20] = "MISO", [22] = "SCLK", [23] = "CE0", \
                        [24] = "GND", [25] = "CE1", [26] = "ID_SD", [27] = "ID_SC", [29] = "GND", [31] = "PWM0", \
                        [32] = "PWM1", [33] = "GND", [34] = "PCM_FS", [37] = "PCM_DIN", [38] = "GND", [39] = "PCM_DOUT"};

Line_list new_list(void)
{
    Line_list rpi = malloc(sizeof(struct line));
    if (!rpi) {
        perror("failed to allocate memory for Line_list.");
        return NULL;
    }
    rpi->pin = 0;
    rpi->gpio = -2;
    rpi->line_fx = NULL;
    rpi->consumer = NULL;
    rpi->next = NULL;
    
    return rpi;
}

Line_list add_pin(Line_list rpi, const uint8_t pin, const int8_t gpio, const char *line_fx)
{
    Line_list rpi_temp = new_list();
    if (!rpi_temp) {
        return NULL;
    }

    rpi_temp->pin = pin;
    rpi_temp->gpio = gpio;
    if (line_fx) {
        rpi_temp->line_fx = malloc(strlen(line_fx) + 1);
        if (!rpi_temp->line_fx) {
            perror("failed to allocate memory for line_fx");
            return NULL;
        }
        if (!strcpy(rpi_temp->line_fx, line_fx)) {
            perror("failed to copy line_fx");
            return NULL;
        }
    }

    if (rpi) {
        rpi_temp->next = rpi;
        rpi = rpi_temp;
    } else {
        rpi = rpi_temp;
    }

    return rpi;
}

void free_line_list(Line_list rpi)
{
    Line_list rpi_temp;

    while (rpi) {
        rpi_temp = rpi;
        rpi = rpi->next;
        free(rpi_temp->line_fx);
        free(rpi_temp->consumer);
        free(rpi_temp);
    }
}

Line_list populate_pins(void)
{
    Line_list rpi = NULL;
    int8_t count;

    for (count = NUM_PINS - 1; count >= 0; count--) {
        rpi = add_pin(rpi, count + 1, gpio_list[count], fx_list[count]);
    }
    
    return rpi;
}

int print_pins(Line_list rpi, enum print format)
{
    Line_list temp;

    // test
    if (!format) {
        printf("\n pin\t gpio\t function\n\n");
        for (temp = rpi; temp; temp = temp->next) {
            if (temp->gpio >= 0 && temp->line_fx) {
                printf("%4.2d\t%4d\t%8s\n", temp->pin, temp->gpio, temp->line_fx);
            } else if (temp->gpio >= 0) {
                printf("%4.2d\t%4d\t%8s\n", temp->pin, temp->gpio, "---");
            } else if (temp->line_fx) {
                printf("%4.2d\t%4s\t%8s\n", temp->pin, "---", temp->line_fx);
            }
        }
    }

    return 0;
}