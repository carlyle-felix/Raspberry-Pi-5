#include <stdio.h>
#include "../include/gpiod-utils.h"

/*
    open gpio chip.

    parameters:
    `str`: chip name.
*/
struct gpiod_chip *chip_open(const char *str)
{
    struct gpiod_chip *chip;

    chip = gpiod_chip_open(str);
    if (!chip) {
        perror("failed to open chip.");
        return NULL;
    }

    return chip;
}

/*
    preconfigure line state.

    parameters:
    `direction`: input/output (enum gpiod_line_direction).
    `initial_state`: active/inactive  (enum gpiod_line_value).
*/
struct gpiod_line_settings *line_state(enum gpiod_line_direction direction, enum gpiod_line_value initial_state)
{
    struct gpiod_line_settings *line_settings;
    uint8_t result;

    line_settings = gpiod_line_settings_new();
    if (!line_settings) {
        perror("failed to creating new line settings object.");
        return NULL;
    }
    result = gpiod_line_settings_set_direction(line_settings, direction);
    if (result) {
        perror("failed to set line direction.");
        gpiod_line_settings_free(line_settings);
        return NULL;
    }
    result = gpiod_line_settings_set_output_value(line_settings, initial_state);
    if (result) {
        perror("failed to set intitial value.");
        gpiod_line_settings_free(line_settings);
        return NULL;
    }

    return line_settings;
}

/*
    select gpio lines and configure line state of selected pins.

    parameters:
    `gpio_list`: int array of gpio pins to configure.
    `num_pins`: number of pins selected in `gpio_list`.
    `line_state`: preconfigured line state (struct gpio_line_settings *line_state()).
*/
struct gpiod_line_config *configure_lines(const unsigned int *gpio_list, const uint8_t num_pins, struct gpiod_line_settings *line_state)
{
    struct gpiod_line_config *line_config;
    uint8_t result;

    line_config = gpiod_line_config_new();
    if (!line_config) {
        perror("failed to create new line");
        return NULL;
    }
    result = gpiod_line_config_add_line_settings(line_config, gpio_list, num_pins, line_state);
    if (result) {
        perror("failed to configure line");
        gpiod_line_config_free(line_config);
        return NULL;
    }
    
    return line_config;
}

/*
    request configured lines.

    parameters:
    `chip`: GPIO chip.
    `line_config`: configured lines struct.
    `request_config`: pointer to request_config struct *
    `consumer`: name of process lines are assigned to (arbitrary).
*/
struct gpiod_line_request *request_lines(struct gpiod_chip *chip, struct gpiod_line_config *line_config, \
                                        struct gpiod_request_config **request_config, const char *consumer)
{
    struct gpiod_line_request *request;

    *request_config = gpiod_request_config_new();
    if (!*request_config) {
        perror("failed to create request config.");
        return NULL;
    }
    gpiod_request_config_set_consumer(*request_config, consumer);
    
    request = gpiod_chip_request_lines(chip, *request_config, line_config);
    if (!request) {
        perror("request for lines failed.");
        gpiod_request_config_free(*request_config);
        return NULL;
    }

    return request;
}