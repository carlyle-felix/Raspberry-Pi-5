#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/gpiod-utils.h"

struct gpio_pin {
    unsigned int gpio_line[1];
    char *consumer;
    struct gpiod_line_settings *line_settings;
    struct gpiod_line_config *line_config;
    struct gpiod_request_config *request_config;
    struct gpiod_line_request *request;
};

/*
    create new line data list for pin.

    parameters:
    `gpio_line`: gpio line number.
    `consumer`: name of process lines are assigned to (arbitrary).
    
    note: return variable must be freed by caller with release_line_items().
*/
Gpio_pin new_line(const unsigned int gpio_line, const char *consumer)
{
    Gpio_pin pin = malloc(sizeof(struct gpio_pin));
    if (!pin) {
        perror("failed to allocate space for pin data list");
        return NULL;
    }

    pin->gpio_line[0] = gpio_line;
    if (consumer) {
        pin->consumer = malloc(strlen(consumer) + 1);
        if (!pin->consumer) {
            perror("failed to allocate memory for pin->consumer");
            return NULL;
        }
        strcpy(pin->consumer, consumer);
    } else {
        pin->consumer = malloc(strlen(DEFAULT_CONSUMER) + 1);
        if (!pin->consumer) {
            perror("failed to allocate memory for pin->consumer");
            return NULL;
        }
        strcpy(pin->consumer, DEFAULT_CONSUMER);
    }
    pin->line_settings = NULL;
    pin->line_config = NULL;
    pin->request_config = NULL;
    pin->request = NULL;

    return pin;
}

/*
    set pin state variables

    parameters:
    `chip`: chip name (/dev/gpiochip0).
    `pin`: line data list for pin.
    `direction`: input/output (enum gpiod_line_direction).
    `initial_state`: active/inactive  (enum gpiod_line_value).

    note: return variable must be freed by caller with release_line_items().
*/
struct gpiod_line_request *pin_setup(struct gpiod_chip *chip, Gpio_pin *pin, enum gpiod_line_direction direction, enum gpiod_line_value initial_state)
{
    (*pin)->line_settings = line_state(direction, initial_state);
    if (!((*pin)->line_settings)) {
        return NULL;
    }

    (*pin)->line_config = configure_lines(pin);
    if (!((*pin)->line_config)) {
        return NULL;
    }

    (*pin)->request = request_lines(chip, pin);
    if (!((*pin)->request)) {
        return NULL;
    }

    return (*pin)->request;
}

/*
    free line data list for pin

    parameters:
    `pin`: line data list for pin.
*/
void release_line_items(Gpio_pin pin) 
{
    free(pin->consumer);
    gpiod_line_settings_free(pin->line_settings);
    gpiod_line_config_free(pin->line_config);
    gpiod_request_config_free(pin->request_config);
    gpiod_line_request_release(pin->request);
    free(pin);
}

/*
    open gpio chip.

    parameters:
    `str`: chip name.

    note: return struct must be freed by caller with gpiod_chip_close().
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

    note: return struct must be freed by caller with gpiod_line_settings_free().
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
    `pin`: line data list for pin.

    note: return struct must be freed by caller with gpiod_line_config_free().
*/
struct gpiod_line_config *configure_lines(Gpio_pin *pin)
{
    struct gpiod_line_config *line_config;
    uint8_t result;

    line_config = gpiod_line_config_new();
    if (!line_config) {
        perror("failed to create new line");
        return NULL;
    }
    result = gpiod_line_config_add_line_settings(line_config, (*pin)->gpio_line, 1, (*pin)->line_settings);
    if (result) {
        perror("failed to configure line");
        return NULL;
    }
    
    return line_config;
}

/*
    request configured lines.

    parameters:
    `chip`: GPIO chip.
    `pin`: line data list for pin.

    notes: 
        return struct must be freed by caller with gpiod_line_request_release().
        `pin` must be freed by caller with release_line_items().
*/
struct gpiod_line_request *request_lines(struct gpiod_chip *chip, Gpio_pin *pin)
{
    struct gpiod_line_request *request;

    (*pin)->request_config = gpiod_request_config_new();
    if (!(*pin)->request_config) {
        perror("failed to create request config.");
        return NULL;
    }
    gpiod_request_config_set_consumer((*pin)->request_config, (*pin)->consumer);
    
    request = gpiod_chip_request_lines(chip, (*pin)->request_config, (*pin)->line_config);
    if (!request) {
        perror("request for lines failed.");
        return NULL;
    }

    return request;
}