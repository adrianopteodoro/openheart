// openheart/main.c
// Entry point for Open Heart (RP2040) project

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include "hardware/watchdog.h"
#include "clock_control.h"
#include "display.h"
#include "controller.h"
#include "structs.h"
// #include "region_switch.h"
// #include "reset_button.h"
// #include "config_store.h"

#define LED_PIN 25 ///< Onboard LED pin

/**
 * @brief Global system status structure.
 *        Holds current region, overclocking state, and joypad state.
 */
system_status_t system_status = {
    .region = REGION_JPN,      // Default region
    .overclocked = false,      // Default overclocking state
    .pad = {0}                 // Initialize joypad state to zero
};

/**
 * @brief Core 1 entry point.
 *        Handles background tasks such as reading the joypad state.
 */
void core1_entry()
{
    while (true)
    {
        // Read the joypad state into the global system_status
        read_genesis_joypad(&system_status.pad);
        sleep_ms(10);
    }
}

/**
 * @brief Main entry point for the application.
 *        Initializes hardware, launches core 1, and updates the display.
 */
int main()
{
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Initialize the master clock output for the initial region
    init_clock_output(system_status.region);

    // Initialize GPIOs for controller input
    genesis_controller_gpio_init();

    // Launch core 1 for background tasks (e.g., joypad polling)
    multicore_launch_core1(core1_entry);

    // Initialize and show the SEGA logo on the OLED display
    display_init();
    display_show_sega_logo();

    // load_config(); // Uncomment if persistent config is implemented

    sleep_ms(2500); // Allow time for peripherals to stabilize

    // Main loop: update display with current status
    while (true)
    {
        display_update_status(system_status.region, system_status.overclocked, system_status.pad);
        sleep_ms(100); // Update display every 100ms
    }
}
