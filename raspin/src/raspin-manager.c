#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <gpiod.h>

#include "../include/raspin-manager.h"
#include "../include/gpiod-utils.h"

#define RED_BG      "\033[41m"
#define DGRAY_BG    "\033[40m"
#define GREEN       "\033[1;92m"
#define DEFAULT     "\033[0m"

struct line {
    uint8_t pin;
    int8_t gpio;
    char *line_fx;
    bool state;
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
    rpi->gpio = -1;
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

Line_list get_line_info(const char *chip_dev, Line_list rpi)
{
    Gpio_pin_info pin;
    Line_list rpi_temp;

    for (rpi_temp = rpi; rpi_temp; rpi_temp = rpi_temp->next) {
        if (rpi_temp->gpio < 0) {
            continue;
        }
        pin = line_info(chip_dev, rpi_temp->gpio);
        if (!pin) {
            perror("failed to get line info in get_line_info()");
            return NULL;
        }

        rpi_temp->state = pin->state;
        
        if (pin->consumer) {
            rpi_temp->consumer = malloc(strlen(pin->consumer) + 1);
            if (!rpi_temp->consumer) {
                perror("failed to allocate memory for consumer in get_line_info()");
                return NULL;
            }
            strcpy(rpi_temp->consumer, pin->consumer);
        } else {
            rpi_temp->consumer = malloc(strlen(NO_CONSUMER) + 1);
            if (!rpi_temp->consumer) {
                perror("failed to allocate memory for consumer in get_line_info()");
                return NULL;
            }
            strcpy(rpi_temp->consumer, NO_CONSUMER);
        }
        release_line_info_items(pin);
        pin = NULL;
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

void print_pins(const char *chip_dev, Line_list rpi, enum print format)
{
    Line_list rpi_temp;
    uint8_t count;

    rpi = get_line_info(chip_dev, rpi);

    printf("\n");
    switch (format) {
        case PRINT_LIST:
            printf(" pin\t gpio\t function\tconsumer\tstate\n\n");
            for (rpi_temp = rpi; rpi_temp; rpi_temp = rpi_temp->next) {
                printf(DEFAULT);
                if (rpi_temp->state) {
                    printf(GREEN);
                }
                if (rpi_temp->gpio >= 0 && rpi_temp->line_fx) {
                    printf("%4.2d\t", rpi_temp->pin);
                    printf("%4d\t", rpi_temp->gpio);
                    printf("%8s\t", rpi_temp->line_fx);
                    printf("%8s\t", rpi_temp->consumer);
                    printf("%s\n", rpi_temp->state ? "used" : "not used");
                } else if (rpi_temp->gpio >= 0) {
                    printf("%4.2d\t", rpi_temp->pin);
                    printf("%4d\t", rpi_temp->gpio);
                    printf("%8s\t", "---");
                    printf("%8s\t", rpi_temp->consumer);
                    printf("%s\n", rpi_temp->state ? "used" : "not used");
                } else if (rpi_temp->line_fx) {
                    printf("%4.2d\t", rpi_temp->pin);
                    printf("%4s\t", "---");
                    printf("%8s\t", rpi_temp->line_fx);
                    printf("%8s\t", "");
                    printf("%s\n", rpi_temp->state ? "used" : "not used");
                }
            }
            break;

        case PRINT_LAYOUT:
            for(count = 0, rpi_temp = rpi; rpi_temp; rpi_temp = rpi_temp->next, count++) {
                printf(DEFAULT);
                if (rpi_temp->state) {
                    printf(GREEN);
                }
                if (!(count % 2)) {
                    if (rpi_temp->gpio >= 0 && rpi_temp->line_fx) {
                        printf("%18s\t", rpi_temp->line_fx);
                        printf("GPIO %2d\t", rpi_temp->gpio);
                        printf("[%.2d] ", rpi_temp->pin);
                    } else if (rpi_temp->gpio >= 0) {
                        printf("\t\t\tGPIO %2d\t", rpi_temp->gpio);
                        printf("[%.2d] ", rpi_temp->pin);
                    } else if (rpi_temp->line_fx) {
                        printf("%31s\t", rpi_temp->line_fx);
                        if (!strcmp(rpi_temp->line_fx, "GND")) {
                            printf(DGRAY_BG);
                        } else {
                            printf(RED_BG);
                        }
                        printf("[%.2d]"DEFAULT" ", rpi_temp->pin);
                    }
                } else {
                    if (rpi_temp->gpio >= 0 && rpi_temp->line_fx) {
                        printf("[%.2d] ", rpi_temp->pin);
                        printf("GPIO %2d      ", rpi_temp->gpio);
                        printf("%s\n", rpi_temp->line_fx);
                    } else if (rpi_temp->gpio >= 0) {
                        printf("[%.2d] ", rpi_temp->pin);
                        printf("GPIO %d\n", rpi_temp->gpio);
                    } else if (rpi_temp->line_fx) {
                        if (!strcmp(rpi_temp->line_fx, "GND")) {
                            printf(DGRAY_BG);
                        } else {
                            printf(RED_BG);
                        }
                        printf("[%.2d]", rpi_temp->pin);
                        printf(DEFAULT);
                        printf(" %s\n", rpi_temp->line_fx);
                    }
                }
            }
            printf(DEFAULT);
            break;

        default:
            break;
    }
    printf("\n");
}