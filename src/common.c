#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "common.h"

void blink_led(uint led_pin, uint32_t on_ms, uint32_t off_ms) {
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);
    while (1) {
        gpio_put(led_pin, 1);
        sleep_ms(on_ms);
        gpio_put(led_pin, 0);
        sleep_ms(off_ms);
    }
}