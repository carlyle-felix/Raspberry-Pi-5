#include <stdio.h>
#include <stdint.h>
#include <unistd.h> // sleep
#include <gpiod.h>

#define CONSUMER "TEST_LED"

// pin setup
#define GPIO_PIN 17 // physical pin 11
#define NUM_PINS 1
const unsigned int gpio_pins[NUM_PINS] = {17};

int main(void) {
    struct gpiod_chip *chip;
    struct gpiod_line_request *request;
    struct gpiod_line_config *line_config;
    struct gpiod_line_settings *line_settings;
    struct gpiod_request_config *request_config;
    uint8_t result, n;

    printf("open chip.\n");
    chip = gpiod_chip_open("/dev/gpiochip0");
    if (!chip) {
        perror("failed to open chip.");
        return 1;
    }

    line_settings = gpiod_line_settings_new();
    if (!line_settings) {
        perror("failed to creating new line settings object.");
        gpiod_chip_close(chip);
        return 1;
    }
    result = gpiod_line_settings_set_direction(line_settings, GPIOD_LINE_DIRECTION_OUTPUT);
    if (result) {
        perror("failed to set line direction.");
        gpiod_line_settings_free(line_settings);
        gpiod_chip_close(chip);
        return 1;
    }
    result = gpiod_line_settings_set_output_value(line_settings, GPIOD_LINE_VALUE_INACTIVE);
    if (result) {
        perror("failed to set intitial value.");
        gpiod_line_settings_free(line_settings);
        gpiod_chip_close(chip);
        return 1;
    }

    line_config = gpiod_line_config_new();
    if (!line_config) {
        perror("failed to create new line");
        gpiod_line_settings_free(line_settings);
        gpiod_chip_close(chip);
        return 1;
    }
    result = gpiod_line_config_add_line_settings(line_config, gpio_pins, NUM_PINS, line_settings);
    if (result) {
        perror("failed to configure line");
        gpiod_line_settings_free(line_settings);
        gpiod_line_config_free(line_config);
        gpiod_chip_close(chip);
        return 1;
    }

    request_config = gpiod_request_config_new();
    if (!request_config) {
        perror("failed to create request config.");
        gpiod_line_settings_free(line_settings);
        gpiod_line_config_free(line_config);
        gpiod_chip_close(chip);
        return 1;
    }
    gpiod_request_config_set_consumer(request_config, CONSUMER);
    
    request = gpiod_chip_request_lines(chip, request_config, line_config);
    if (!request) {
        perror("request for lines failed.");
        gpiod_line_settings_free(line_settings);
        gpiod_line_config_free(line_config);
        gpiod_request_config_free(request_config);
        gpiod_chip_close(chip);
    }

    for (n = 0; n < 10; n++) {
        printf("state: HIGH.\n");
        if (gpiod_line_request_set_value(request, GPIO_PIN, GPIOD_LINE_VALUE_ACTIVE)) {
            perror("failed to change GPIO pin state");
        }
        sleep(1);
        
        printf("state: LOW.\n");
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

    return 0;
}