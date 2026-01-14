#ifndef GPIOD_UTILS_H
#define GPIOD_UTILS_H

#include <stdint.h>
#include <gpiod.h>
#include <stdbool.h>

#define DEFAULT_CONSUMER "gpiod-utils"

typedef struct gpio_pin *Gpio_pin;
typedef struct gpio_pin_info {
    bool state;
    char *consumer;
} *Gpio_pin_info;

Gpio_pin new_line(const unsigned int gpio_line, const char *consumer);
struct gpiod_line_request *line_request(struct gpiod_chip *chip, Gpio_pin *pin, enum gpiod_line_direction direction, enum gpiod_line_value initial_state);
void release_line_items(Gpio_pin pin);
void release_line_info_items(Gpio_pin_info pin);
struct gpiod_chip *open_chip(const char *str);
struct gpiod_line_settings *line_state(enum gpiod_line_direction direction, enum gpiod_line_value initial_state);
struct gpiod_line_config *configure_line(Gpio_pin *pin);
struct gpiod_line_request *request(struct gpiod_chip *chip, Gpio_pin *pin);
Gpio_pin_info line_info(const char *chip_dev, const uint8_t gpio_line);

#endif