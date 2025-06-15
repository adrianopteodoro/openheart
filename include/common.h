#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

// Blink the specified LED pin forever with custom on/off times (in ms)
void blink_led(uint led_pin, uint32_t on_ms, uint32_t off_ms);

// Timing definitions for error blinking
#define BLINK_ERROR_FAST_ON  200
#define BLINK_ERROR_FAST_OFF 200
#define BLINK_ERROR_SLOW_ON  100
#define BLINK_ERROR_SLOW_OFF 900

#endif // COMMON_H