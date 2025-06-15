#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <stdint.h>
#include <stdbool.h>
#include "structs.h"
#include "hardware/gpio.h"
#include "setup.h"

// Setup GPIOs for controller
void genesis_controller_gpio_init(void);

// Helper to set the select line (pin 9)
void set_select_line(int value);

// Helper to read the 6 data lines (Up, Down, Left, Right, B, C)
uint8_t read_data_lines(void);

// Read Genesis controller and fill joypad_state_t
void read_genesis_joypad(joypad_state_t *pad);

#endif  // CONTROLLER_H
