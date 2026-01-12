#include <stdio.h>
#include <unistd.h> // sleep

#include "../include/gpiod-utils.h"

#define CHIP "/dev/gpiochip0"
#define LED_CONSUMER "clap-led: LED"
#define LED_PIN11 17
#define SENSOR_CONSUMER "clap-led: sound sensor"
#define SENSOR_PIN16 23

int main(void)
{
    struct gpiod_chip *chip;
    struct gpiod_line_request *sensor_request, *led_request;
    Gpio_pin sensor, led;

    // chip setup
    chip = chip_open(CHIP);
    if (!chip) {
        return 1;
    }

    // sensor setup
    sensor = new_line(SENSOR_PIN16, SENSOR_CONSUMER);
    if (!sensor) {
        gpiod_chip_close(chip);
        return 1;
    }
    sensor_request = pin_setup(chip, &sensor, GPIOD_LINE_DIRECTION_INPUT, GPIOD_LINE_VALUE_INACTIVE);
    if (!sensor) {
        release_line_items(sensor);
        gpiod_chip_close(chip);
    }

    // led setup
    led = new_line(LED_PIN11, LED_CONSUMER);
    if (!led) {
        release_line_items(sensor);
        gpiod_chip_close(chip);
        return 1;
    }
    led_request = pin_setup(chip, &led, GPIOD_LINE_DIRECTION_OUTPUT, GPIOD_LINE_VALUE_INACTIVE);
    if (!led) {
        release_line_items(led);
        release_line_items(sensor);
        gpiod_chip_close(chip);
    }

    // begin logic
    while (!gpiod_line_request_get_value(sensor_request, SENSOR_PIN16)) {
        usleep(25000);
    }
    printf("sound detected.\n");

    if (gpiod_line_request_set_value(led_request, LED_PIN11, GPIOD_LINE_VALUE_ACTIVE)) {
        perror("failed to change GPIO pin state.");
    }
    sleep(1);
    if (gpiod_line_request_set_value(led_request, LED_PIN11, GPIOD_LINE_VALUE_INACTIVE)) {
        perror("failed to change GPIO pin state.");
    }

    // free
    release_line_items(led);
    release_line_items(sensor);
    gpiod_chip_close(chip);

    return 1;
}