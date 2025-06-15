#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/pll.h"
#include "hardware/pwm.h"
#include "clock_control.h"
#include "setup.h"

/**
 * @brief Holds PLL and divider settings for a video region.
 */
typedef struct
{
    uint32_t pll_sys_hz; ///< PLL system frequency in Hz
    uint8_t postdiv1;    ///< First post-divider
    uint8_t postdiv2;    ///< Second post-divider
} region_clock_t;

// Table of region clock settings (PLL in Hz and dividers)
// These values are chosen to generate frequencies as close as possible to video standards
static const region_clock_t region_clocks[] = {
    [REGION_JPN] = {1074 * MHZ, 5, 2}, // 53.700000 MHz (NTSC/JPN/USA)
    [REGION_USA] = {1074 * MHZ, 5, 2}, // 53.700000 MHz (NTSC/JPN/USA)
    [REGION_EUR] = {1596 * MHZ, 3, 5}, // 53.200000 MHz (PAL/EUR)
    [REGION_BRA] = {1075 * MHZ, 5, 2}, // 53.800000 MHz (BR)
};

/**
 * @brief Get the clock configuration for a given region.
 *        Defaults to NTSC/JPN if region is invalid.
 * @param region The region to look up.
 * @return Pointer to region_clock_t settings.
 */
static inline const region_clock_t *get_region_clock(region_t region)
{
    if (region >= 0 && region < (int)(sizeof(region_clocks) / sizeof(region_clocks[0])))
        return &region_clocks[region];
    return &region_clocks[REGION_JPN];
}

/**
 * @brief Set the system PLL and output clock for the selected region.
 * @param region The region to configure.
 */
void set_clock_region(region_t region)
{
    const region_clock_t *rc = get_region_clock(region);
    set_sys_clock_pll(rc->pll_sys_hz, rc->postdiv1, rc->postdiv2);
    clock_gpio_init(GPIO_MCLK_PIN, CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS, 2);

    // Set VCLK (CPU clock for 68000) divider to 7 for all regions
    setup_vclk_pwm_div(7);
}

/**
 * @brief Initialize the master clock output for the initial region.
 *        Sets drive strength and slew rate for high-frequency output.
 * @param initial_region The region to initialize.
 */
void init_clock_output(region_t initial_region)
{
    gpio_set_drive_strength(GPIO_MCLK_PIN, GPIO_DRIVE_STRENGTH_8MA);
    gpio_set_slew_rate(GPIO_MCLK_PIN, GPIO_SLEW_RATE_FAST);
    set_clock_region(initial_region);
}

/**
 * @brief Configure VCLK (CPU clock for Motorola 68000) using PWM.
 *        VCLK is generated with PWM, sourced from pll_sys (the MCLK source).
 *        The wrap value divides by 2 to get MCLK rate, then the divider sets the final CPU clock.
 * @param div Divider value for the PWM clock.
 */
void setup_vclk_pwm_div(uint32_t div)
{
    gpio_set_function(GPIO_VCLK_PIN, GPIO_FUNC_PWM);

    uint32_t slice = pwm_gpio_to_slice_num(GPIO_VCLK_PIN);

    // Set up a 50% duty cycle PWM based on the system clock/MCLK speed and the specified divider
    pwm_set_clkdiv_int_frac(slice, div, 0);
    pwm_set_clkdiv_mode(slice, PWM_DIV_FREE_RUNNING);
    pwm_set_phase_correct(slice, false);
    pwm_set_wrap(slice, 1); // Wrap value of 1 gives us PWM at MCLK rate
    pwm_set_chan_level(slice, pwm_gpio_to_channel(GPIO_VCLK_PIN), 1);
    pwm_set_enabled(slice, true);
}