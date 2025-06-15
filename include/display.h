#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include "enums.h"
#include "structs.h"

/**
 * @brief Initialize the OLED display and I2C interface.
 */
void display_init(void);

/**
 * @brief Show the SEGA logo bitmap on the display.
 */
void display_show_sega_logo(void);

/**
 * @brief Update the display with region, overclock status, and pad inputs.
 * @param region      The current video region.
 * @param overclocked Overclock status (true if enabled).
 * @param pad         Current joypad state.
 */
void display_update_status(region_t region, bool overclocked, joypad_state_t pad);

/**
 * @brief Draw the region and subcarrier information at the top of the display.
 * @param region The current video region.
 */
void draw_region_and_subcarrier(region_t region);

/**
 * @brief Draw the current state of the joypad inputs on the display.
 * @param pad Current joypad state.
 */
void display_pad_inputs(joypad_state_t pad);

#endif // DISPLAY_H
