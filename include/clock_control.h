#ifndef CLOCK_CONTROL_H
#define CLOCK_CONTROL_H

#include "enums.h"

/**
 * @brief Initialize the master clock output for a given region.
 *        Sets up drive strength, slew rate, and region-specific clock.
 * @param initial_region The region to initialize.
 */
void init_clock_output(region_t initial_region);

/**
 * @brief Change the clock region at runtime.
 *        Updates PLL and output clock for the specified region.
 * @param region The region to configure.
 */
void set_clock_region(region_t region);

/**
 * @brief Configure VCLK PWM divider (for CPU clock output).
 *        Sets the divider for the VCLK PWM output.
 * @param div Divider value for the PWM clock.
 */
void setup_vclk_pwm_div(uint32_t div);

#endif // CLOCK_CONTROL_H
