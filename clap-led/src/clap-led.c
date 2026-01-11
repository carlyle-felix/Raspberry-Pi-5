#include <stdio.h>
#include <unistd.h> // sleep

#include "../include/gpiod-utils.h"

#define CHIP "/dev/gpiochip0"
#define CONSUMER "clap-led"

#define PIN11 17
#define PIN16 23
const unsigned int led[] = {17};
const unsigned int sens[] = {23};


int main(void)
{
    struct gpiod_chip *chip;
    struct gpiod_line_settings *led_line_settings, *sens_line_settings;
    struct gpiod_line_config *led_line_config, *sens_line_config;
    struct gpiod_request_config *led_request_config, *sens_request_config;
    struct gpiod_line_request *led_request, *sens_request;
    uint8_t n;

    chip = chip_open(CHIP);
    if (!chip) {
        return 1;
    }

    led_line_settings = line_state(GPIOD_LINE_DIRECTION_OUTPUT, GPIOD_LINE_VALUE_INACTIVE);
    if (!led_line_settings) {
        gpiod_chip_close(chip);
        return 1;
    }

    led_line_config = configure_lines(led, 1, led_line_settings);
    if (!led_line_config) {
        gpiod_chip_close(chip);
        gpiod_line_settings_free(led_line_settings);
        return 1;
    }

    led_request = request_lines(chip, led_line_config, &led_request_config, CONSUMER);
    if (!led_request) {
        gpiod_chip_close(chip);
        gpiod_line_settings_free(led_line_settings);
        gpiod_line_config_free(led_line_config);
    }

    sens_line_settings = line_state(GPIOD_LINE_DIRECTION_INPUT, GPIOD_LINE_VALUE_INACTIVE);
    if (!sens_line_settings) {
        gpiod_chip_close(chip);
        gpiod_line_settings_free(led_line_settings);
        gpiod_line_config_free(led_line_config);
        gpiod_request_config_free(led_request_config);
        gpiod_line_request_release(led_request);
        return 1;
    }

    sens_line_config = configure_lines(sens, 1, sens_line_settings);
    if (!sens_line_config) {
        gpiod_chip_close(chip);
        gpiod_line_settings_free(led_line_settings);
        gpiod_line_config_free(led_line_config);
        gpiod_request_config_free(led_request_config);
        gpiod_line_request_release(led_request);
        gpiod_line_settings_free(sens_line_settings);
        return 1;
    }

    sens_request = request_lines(chip, sens_line_config, &sens_request_config, CONSUMER);
    if (!sens_request) {
        gpiod_chip_close(chip);
        gpiod_line_settings_free(led_line_settings);
        gpiod_line_config_free(led_line_config);
        gpiod_request_config_free(led_request_config);
        gpiod_line_request_release(led_request);
        gpiod_line_settings_free(sens_line_settings);
        gpiod_line_config_free(sens_line_config);
    }

    for (n = 0; n < 10; n++) {
        while (!gpiod_line_request_get_value(sens_request, PIN16)) {
            usleep(25000);
        }

        printf("sound detected.\n");
        if (gpiod_line_request_set_value(led_request, PIN11, GPIOD_LINE_VALUE_ACTIVE)) {
            perror("failed to change GPIO pin state.");
        }
        sleep(1);
        if (gpiod_line_request_set_value(led_request, PIN11, GPIOD_LINE_VALUE_INACTIVE)) {
            perror("failed to change GPIO pin state.");
        }
    }
    
    gpiod_chip_close(chip);
    gpiod_line_settings_free(led_line_settings);
    gpiod_line_config_free(led_line_config);
    gpiod_request_config_free(led_request_config);
    gpiod_line_request_release(led_request);
    gpiod_line_settings_free(sens_line_settings);
    gpiod_line_config_free(sens_line_config);
    gpiod_request_config_free(sens_request_config);
    gpiod_line_request_release(sens_request);
}