#include <stdio.h>
#include <unistd.h> // sleep

#include "../include/gpiod-utils.h"

#define CHIP "/dev/gpiochip0"
#define CONSUMER "clap-led"

#define MAX_PINS 1
#define PIN11 17
const unsigned int gpio_pins[MAX_PINS] = {17};

int main(void)
{
    struct gpiod_chip *chip;
    struct gpiod_line_settings *line_setings;
    struct gpiod_line_config *line_config;
    struct gpiod_request_config *request_config;
    struct gpiod_line_request *request;
    uint8_t n;

    chip = chip_open(CHIP);
    if (!chip) {
        return 1;
    }

    line_setings = line_state(GPIOD_LINE_DIRECTION_OUTPUT, GPIOD_LINE_VALUE_INACTIVE);
    if (!line_setings) {
        gpiod_chip_close(chip);
        return 1;
    }

    line_config = configure_lines(gpio_pins, MAX_PINS, line_setings);
    if (!line_config) {
        gpiod_chip_close(chip);
        gpiod_line_settings_free(line_setings);
        return 1;
    }

    request = request_lines(chip, line_config, &request_config, CONSUMER);
    if (!request) {
        gpiod_chip_close(chip);
        gpiod_line_settings_free(line_setings);
        gpiod_line_config_free(line_config);
    }

    // logic
    for (n = 0; n < 10; n++) {
        if (gpiod_line_request_set_value(request, PIN11, GPIOD_LINE_VALUE_ACTIVE)) {
            perror("failed to change GPIO pin state");
        }
        sleep(1);
        
        if (gpiod_line_request_set_value(request, PIN11, GPIOD_LINE_VALUE_INACTIVE)) {
            perror("failed to change GPIO pin state");
        }
        sleep(1);
    }

    gpiod_chip_close(chip);
    gpiod_line_settings_free(line_setings);
    gpiod_line_config_free(line_config);
    gpiod_request_config_free(request_config);
    gpiod_line_request_release(request);
}


/*
    // logic
    for (n = 0; n < 10; n++) {
        if (gpiod_line_request_set_value(request, GPIO_PIN, GPIOD_LINE_VALUE_ACTIVE)) {
            perror("failed to change GPIO pin state");
        }
        sleep(1);
        
        if (gpiod_line_request_set_value(request, GPIO_PIN, GPIOD_LINE_VALUE_INACTIVE)) {
            perror("failed to change GPIO pin state");
        }
        sleep(1);
    }

    gpiod_line_settings_free(line_settings);
    gpiod_line_config_free(line_config);
    gpiod_request_config_free(request_config);
    gpiod_chip_close(chip);
    gpiod_line_request_release(request);
*/

