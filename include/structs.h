#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdbool.h>
#include <stdint.h>
#include "enums.h"

/**
 * @brief Represents the state of a joypad's buttons.
 */
typedef struct
{
    bool up;      ///< Up direction pressed
    bool down;    ///< Down direction pressed
    bool left;    ///< Left direction pressed
    bool right;   ///< Right direction pressed
    bool a;       ///< A button pressed
    bool b;       ///< B button pressed
    bool c;       ///< C button pressed
    bool x;       ///< X button pressed
    bool y;       ///< Y button pressed
    bool z;       ///< Z button pressed
    bool start;   ///< Start button pressed
    bool mode;    ///< Mode button pressed
} joypad_state_t;

/**
 * @brief Represents the current status of the system.
 */
typedef struct
{
    region_t region;         ///< Current region setting
    bool overclocked;        ///< Overclocking enabled/disabled
    joypad_state_t pad;      ///< Current joypad state
} system_status_t;

/**
 * @brief Represents persistent configuration data.
 */
typedef struct
{
    uint32_t magic;          ///< Magic number to identify config
    region_t region;         ///< Saved region setting
    bool overclocked;        ///< Overclocking enabled/disabled
} config_t;

/**
 * @brief Represents a single button on a pad, with its state and label.
 */
typedef struct
{
    bool pressed;            ///< True if button is pressed
    const char *label;       ///< Button label (e.g., "A", "Start")
} pad_button_t;

#endif // STRUCTS_H
// End