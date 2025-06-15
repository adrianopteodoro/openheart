#include "controller.h"
#include "structs.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdint.h>
#include <stdbool.h>

/*
 * Sega Genesis/Mega Drive 3/6-Button Controller Interface
 * -------------------------------------------------------
 * This module provides functions to interface with Sega Genesis/Mega Drive
 * 3-button and 6-button controllers using GPIO pins on the Raspberry Pi Pico.
 * It implements the official Sega 6-button protocol for accurate button detection.
 *
 * GPIO_PIN_* macros must be defined in setup.h for each controller line:
 *   GPIO_PIN_UP, GPIO_PIN_DOWN, GPIO_PIN_LEFT, GPIO_PIN_RIGHT,
 *   GPIO_PIN_B, GPIO_PIN_C, GPIO_PIN_SELECT
 */

/**
 * @brief Initialize GPIOs for the Genesis/Mega Drive controller.
 *        Data lines are set as input with pull-ups; select line as output.
 */
void genesis_controller_gpio_init(void) {
    gpio_init(GPIO_PIN_UP);      gpio_set_dir(GPIO_PIN_UP, GPIO_IN);      gpio_pull_up(GPIO_PIN_UP);
    gpio_init(GPIO_PIN_DOWN);    gpio_set_dir(GPIO_PIN_DOWN, GPIO_IN);    gpio_pull_up(GPIO_PIN_DOWN);
    gpio_init(GPIO_PIN_LEFT);    gpio_set_dir(GPIO_PIN_LEFT, GPIO_IN);    gpio_pull_up(GPIO_PIN_LEFT);
    gpio_init(GPIO_PIN_RIGHT);   gpio_set_dir(GPIO_PIN_RIGHT, GPIO_IN);   gpio_pull_up(GPIO_PIN_RIGHT);
    gpio_init(GPIO_PIN_B);       gpio_set_dir(GPIO_PIN_B, GPIO_IN);       gpio_pull_up(GPIO_PIN_B);
    gpio_init(GPIO_PIN_C);       gpio_set_dir(GPIO_PIN_C, GPIO_IN);       gpio_pull_up(GPIO_PIN_C);

    gpio_init(GPIO_PIN_SELECT);  gpio_set_dir(GPIO_PIN_SELECT, GPIO_OUT); gpio_put(GPIO_PIN_SELECT, 1);
}

/**
 * @brief Set the SELECT line (pin 9 on DB9 connector).
 * @param value 1 for HIGH, 0 for LOW.
 */
void set_select_line(int value) {
    gpio_put(GPIO_PIN_SELECT, value ? 1 : 0);
}

/**
 * @brief Read the 6 data lines (Up, Down, Left, Right, B, C).
 * @return Bitfield: bit0=Up, bit1=Down, bit2=Left, bit3=Right, bit4=B, bit5=C.
 *         All lines are active low (pressed = 1).
 */
uint8_t read_data_lines(void) {
    uint8_t data = 0;
    data |= (!gpio_get(GPIO_PIN_UP)    << 0); // Active low: pressed = 0
    data |= (!gpio_get(GPIO_PIN_DOWN)  << 1);
    data |= (!gpio_get(GPIO_PIN_LEFT)  << 2);
    data |= (!gpio_get(GPIO_PIN_RIGHT) << 3);
    data |= (!gpio_get(GPIO_PIN_B)     << 4);
    data |= (!gpio_get(GPIO_PIN_C)     << 5);
    return data;
}

/**
 * @brief Read the Genesis/Mega Drive controller and fill the joypad_state_t struct.
 *        Supports both 3-button and 6-button controllers using the official protocol.
 *        - Only the first SELECT=1 read is trusted for directions.
 *        - X/Y/Z/MODE are only valid after a successful 6-button handshake.
 *        - All timings are based on the official protocol and hardware info.
 *
 * @param pad Pointer to joypad_state_t struct to fill with button states.
 */
void read_genesis_joypad(joypad_state_t *pad) {
    uint8_t data;
    *pad = (joypad_state_t){0}; // Clear all button states

    // 1. SELECT = 1: Read directions, B, C (directions only valid here)
    set_select_line(1); sleep_us(10);
    data = read_data_lines();
    pad->up    = (data & (1 << 0)) != 0;
    pad->down  = (data & (1 << 1)) != 0;
    pad->left  = (data & (1 << 2)) != 0;
    pad->right = (data & (1 << 3)) != 0;
    pad->b     = (data & (1 << 4)) != 0;
    pad->c     = (data & (1 << 5)) != 0;

    // 2. SELECT = 0: Read A, START (ignore directions here)
    set_select_line(0); sleep_us(10);
    data = read_data_lines();
    pad->a     = (data & (1 << 4)) != 0;
    pad->start = (data & (1 << 5)) != 0;

    // 3. Handshake: 4 rapid SELECT transitions (ignore reads)
    for (int i = 0; i < 3; ++i) {
        set_select_line(1); sleep_us(10);
        set_select_line(0); sleep_us(10);
    }
    // Last handshake SELECT=1, then SELECT=0 for detection
    set_select_line(1); sleep_us(10);
    set_select_line(0); sleep_us(10);
    data = read_data_lines();
    bool six_button = ((data & 0x0F) == 0);

    // 4. Final SELECT=1: Only now are X/Y/Z/MODE valid (on direction lines)
    set_select_line(1); sleep_us(10);
    data = read_data_lines();
    if (six_button) {
        pad->z    = (data & (1 << 0)) != 0;
        pad->y    = (data & (1 << 1)) != 0;
        pad->x    = (data & (1 << 2)) != 0;
        pad->mode = (data & (1 << 3)) != 0;
    } else {
        pad->z = pad->y = pad->x = pad->mode = false;
    }
}