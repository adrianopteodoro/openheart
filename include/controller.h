#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>
#include "structs.h"
#include "hardware/gpio.h"
#include "setup.h"

/**
 * @brief Initialize GPIOs for the Genesis/Mega Drive controller.
 *        Data lines are set as input with pull-ups; select line as output.
 */
void genesis_controller_gpio_init(void);

/**
 * @brief Set the SELECT line (pin 9 on DB9 connector).
 * @param value 1 for HIGH, 0 for LOW.
 */
void set_select_line(int value);

/**
 * @brief Pulse the SELECT line: HIGH->LOW with short delay.
 *        Used for 6-button handshake sequence.
 */
void pulse_select_line(void);

/**
 * @brief Read the 6 data lines (Up, Down, Left, Right, B, C).
 *        Also checks if all directions (U/D/L/R) are low at the same time.
 * @return Bitfield: bit0=Up, bit1=Down, bit2=Left, bit3=Right, bit4=B, bit5=C, bit6=AllDirsLow.
 *         All lines are active low (pressed = 1).
 *         bit6 is set if U, D, L, R are all low (pressed) simultaneously.
 */
uint8_t read_data_lines(void);

/**
 * @brief Read Genesis controller and fill joypad_state_t.
 *        Supports both 3-button and 6-button controllers.
 * @param pad Pointer to joypad_state_t struct to fill with button states.
 */
void read_genesis_joypad(joypad_state_t *pad);

#endif  // CONTROLLER_H
