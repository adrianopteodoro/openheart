#ifndef SETUP_H
#define SETUP_H

// Feature toggles
#define ENABLE_OLED_DISPLAY 1    ///< Enable support for OLED display (1 = enable, 0 = disable)
#define ENABLE_OVERCLOCKING 1    ///< Enable support for system overclocking (1 = enable, 0 = disable)

/**
 * @brief Joystick DB9 pinout reference (viewed from plug):
 *
 *      5 4 3 2 1
 *       9 8 7 6
 *
 *  1 - Up / Z
 *  2 - Down / Y
 *  3 - Left / Y
 *  4 - Right / Mode
 *  5 - +5V
 *  6 - B / A
 *  7 - Select line
 *  8 - Ground
 *  9 - C / Start
 */

// GPIO pin assignments for controller lines
#define GPIO_PIN_UP      2   ///< DB9 pin 1
#define GPIO_PIN_DOWN    3   ///< DB9 pin 2
#define GPIO_PIN_LEFT    4   ///< DB9 pin 3
#define GPIO_PIN_RIGHT   5   ///< DB9 pin 4
#define GPIO_PIN_B       6   ///< DB9 pin 6
#define GPIO_PIN_C       7   ///< DB9 pin 9
#define GPIO_PIN_SELECT  8   ///< DB9 pin 7 (Select line, output)

// GPIO pin used for VCLK output (CPU clock or overclocking)
#define GPIO_VCLK_PIN    20  ///< VCLK output pin

// GPIO pin used for master clock output (MCLK)
#define GPIO_MCLK_PIN    21  ///< Master clock output pin

// OLED display I2C pin assignments
#define OLED_SCL_PIN     16  ///< OLED I2C clock (SCL)
#define OLED_SDA_PIN     17  ///< OLED I2C data (SDA)

// GPIO pin used for error LED indication (typically onboard LED on Pico)
#define ERROR_LED_PIN    25  ///< Error LED pin

#endif // SETUP_H
// End of setup.h
