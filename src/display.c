#include "setup.h"
#include "display.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "sega_logo.h"
#include <string.h>
#include <stdio.h>
#include "pico-ssd1306/ssd1306.h"

#define I2C_PORT i2c0

static ssd1306_t display;

/**
 * @brief Draw a monochrome bitmap (1bpp, 8 pixels per byte, row-major).
 * @param disp   Pointer to the display object.
 * @param x0     X coordinate to start drawing.
 * @param y0     Y coordinate to start drawing.
 * @param w      Width of the bitmap.
 * @param h      Height of the bitmap.
 * @param bitmap Pointer to bitmap data.
 */
static void ssd1306_draw_bitmap(ssd1306_t *disp, int x0, int y0, int w, int h, const uint8_t *bitmap)
{
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int byte_index = (y * ((w + 7) / 8)) + (x / 8);
            int bit_index = 7 - (x % 8);
            uint8_t byte = bitmap[byte_index];
            int pixel_on = (byte >> bit_index) & 0x1;
            if (pixel_on)
                ssd1306_draw_pixel(disp, x0 + x, y0 + y);
        }
    }
}

/**
 * @brief Initialize the OLED display and I2C interface.
 */
void display_init(void)
{
    if (!ENABLE_OLED_DISPLAY)
        return;

    // Initialize I2C at 400kHz
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(OLED_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(OLED_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(OLED_SDA_PIN);
    gpio_pull_up(OLED_SCL_PIN);

    // Initialize the SSD1306 display (128x64, I2C address 0x3C)
    ssd1306_init(&display, 128, 64, 0x3C, I2C_PORT);
    ssd1306_clear(&display);
    ssd1306_show(&display);
}

/**
 * @brief Show the SEGA logo bitmap on the display.
 */
void display_show_sega_logo(void)
{
    if (!ENABLE_OLED_DISPLAY)
        return;

    ssd1306_draw_bitmap(&display, 0, 0, 128, 64, sega_logo_bitmap);
    ssd1306_show(&display);
    sleep_ms(2000); // Wait for 2 seconds
    ssd1306_clear(&display);
    ssd1306_show(&display);
}

/**
 * @brief Update the display with region, overclock status, and pad inputs.
 * @param region      The current video region.
 * @param overclocked Overclock status (true if enabled).
 * @param pad         Current joypad state.
 */
void display_update_status(region_t region, bool overclocked, joypad_state_t pad)
{
    if (!ENABLE_OLED_DISPLAY)
        return;

    ssd1306_clear(&display);

    draw_region_and_subcarrier(region);

    char oc_buf[16];
    snprintf(oc_buf, sizeof(oc_buf), "OC: %s", overclocked ? "ON" : "OFF");
    ssd1306_draw_string(&display, 0, 20, 1, oc_buf);

    display_pad_inputs(pad);

    ssd1306_show(&display);
}

/**
 * @brief Draw the region and subcarrier information at the top of the display.
 * @param region The current video region.
 */
void draw_region_and_subcarrier(region_t region)
{
    const char *region_str = (region == REGION_USA) ? "USA" :
                             (region == REGION_EUR) ? "EUR" :
                             (region == REGION_BRA) ? "BRA" : "JPN";
    const char *clock_str = (region == REGION_JPN || region == REGION_USA) ? "NTSC" :
                            (region == REGION_BRA) ? "PAL-M" : "PAL";
    char line[32];
    snprintf(line, sizeof(line), "%s / %s", region_str, clock_str);
    ssd1306_draw_string(&display, 0, 0, 1, line);
}

/**
 * @brief Draw the current state of the joypad inputs on the display.
 * @param pad Current joypad state.
 */
void display_pad_inputs(joypad_state_t pad)
{
    pad_button_t pad_buttons[] = {
        {pad.up, "^"},
        {pad.down, "v"},
        {pad.left, "<"},
        {pad.right, ">"},
        {pad.a, "A"},
        {pad.b, "B"},
        {pad.c, "C"},
        {pad.start, "START"},
        {pad.x, "X"},
        {pad.y, "Y"},
        {pad.z, "Z"},
        {pad.mode, "MODE"}
    };

    char pad_buf[48] = "PAD1:";
    size_t pad_buf_len = strlen(pad_buf);
    size_t pad_buf_remaining = sizeof(pad_buf) - pad_buf_len;

    bool any_button_pressed = false;

    // Append pressed button labels to the buffer
    for (size_t i = 0; i < sizeof(pad_buttons) / sizeof(pad_buttons[0]); ++i)
    {
        if (pad_buttons[i].pressed)
        {
            int written = snprintf(pad_buf + pad_buf_len, pad_buf_remaining, " %s", pad_buttons[i].label);
            if (written < 0 || (size_t)written >= pad_buf_remaining)
            {
                break;
            }
            pad_buf_len += written;
            pad_buf_remaining -= written;
            any_button_pressed = true;
        }
    }

    // If no button is pressed, indicate so
    if (!any_button_pressed)
    {
        snprintf(pad_buf + pad_buf_len, pad_buf_remaining, " (none)");
    }
    ssd1306_draw_string(&display, 0, 30, 1, pad_buf);
}
