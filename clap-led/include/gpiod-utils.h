#ifndef GPIOD_UTILS_H
#define GPIOD_UTILS_H

#include <stdint.h>
#include <gpiod.h>

struct gpiod_chip *chip_open(const char *str);
struct gpiod_line_settings *line_state(enum gpiod_line_direction direction, enum gpiod_line_value initial_state);
struct gpiod_line_config *configure_lines(const unsigned int *pins, const uint8_t num_pins, struct gpiod_line_settings *line_settings);
struct gpiod_line_request *request_lines(struct gpiod_chip *chip, struct gpiod_line_config *line_config, \
                                        struct gpiod_request_config **request_config, const char *consumer);

#endif