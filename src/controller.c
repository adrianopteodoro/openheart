#include "controller.h"
#include "structs.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdint.h>
#include <stdbool.h>

#define LOW 0
#define HIGH 1

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

    gpio_init(GPIO_PIN_SELECT);  gpio_set_dir(GPIO_PIN_SELECT, GPIO_OUT); gpio_put(GPIO_PIN_SELECT, HIGH);
}

/**
 * @brief Set the SELECT line (pin 9 on DB9 connector).
 * @param value 1 for HIGH, 0 for LOW.
 */
void set_select_line(int value) {
    gpio_put(GPIO_PIN_SELECT, value ? HIGH : LOW);
}

/**
 * @brief Pulse the SELECT line: HIGH->LOW with short delay.
 *        Used for 6-button handshake sequence.
 */
void pulse_select_line(void) {
    set_select_line(HIGH);
    sleep_us(10);
    set_select_line(LOW);
    sleep_us(10);
}

/**
 * @brief Read the 6 data lines (Up, Down, Left, Right, B, C).
 *        Also checks if all directions (U/D/L/R) are low at the same time.
 * @return Bitfield: bit0=Up, bit1=Down, bit2=Left, bit3=Right, bit4=B, bit5=C, bit6=AllDirsLow.
 *         All lines are active low (pressed = 1).
 *         bit6 is set if U, D, L, R are all low (pressed) simultaneously.
 */
uint8_t read_data_lines(void) {
    uint8_t data = 0;
    bool up    = !gpio_get(GPIO_PIN_UP);
    bool down  = !gpio_get(GPIO_PIN_DOWN);
    bool left  = !gpio_get(GPIO_PIN_LEFT);
    bool right = !gpio_get(GPIO_PIN_RIGHT);

    data |= (up    << 0); // Up
    data |= (down  << 1); // Down
    data |= (left  << 2); // Left
    data |= (right << 3); // Right
    data |= (!gpio_get(GPIO_PIN_B)     << 4); // B
    data |= (!gpio_get(GPIO_PIN_C)     << 5); // C

    // Set bit 6 if all directions are pressed (all low)
    data |= ((up && down && left && right) << 6);

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
    set_select_line(HIGH); sleep_us(10);
    data = read_data_lines();
    pad->up    = (data & (1 << 0)) != 0;
    pad->down  = (data & (1 << 1)) != 0;
    pad->left  = (data & (1 << 2)) != 0;
    pad->right = (data & (1 << 3)) != 0;
    pad->b     = (data & (1 << 4)) != 0;
    pad->c     = (data & (1 << 5)) != 0;

    // 2. SELECT = 0: Read A, START (ignore directions here)
    set_select_line(LOW); sleep_us(10);
    data = read_data_lines();
    pad->a     = (data & (1 << 4)) != 0;
    pad->start = (data & (1 << 5)) != 0;

    // 3. Handshake: 2 rapid SELECT transitions, then read for 6-button detection
    pulse_select_line();
    pulse_select_line();
    data = read_data_lines();
    // Use bit 6 from read_data_lines to detect 6-button controller
    bool six_button = (data & (1 << 6)) != 0;

    // 4. Final SELECT=1: Only now are X/Y/Z/MODE valid (on direction lines)
    set_select_line(HIGH); sleep_us(10);
    data = read_data_lines();
    if (six_button) {
        pad->z    = (data & (1 << 0)) != 0;
        pad->y    = (data & (1 << 1)) != 0;
        pad->x    = (data & (1 << 2)) != 0;
        pad->mode = (data & (1 << 3)) != 0;
    } else {
        pad->x = pad->y = pad->z = pad->mode = false;
    }
    set_select_line(LOW); sleep_us(10);
    set_select_line(HIGH); sleep_us(10);
}
