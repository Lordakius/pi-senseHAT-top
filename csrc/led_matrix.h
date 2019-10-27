/*
 * led_matrix.h
 *
 * This file contains declarations of convenient functions for activating
 * the LED matrix on the Raspberry Pi Sense HAT (astropi).
 *
 * The details of opening the framebuffer device and mapping it into memory
 * are hidden in the functions open_led_matrix() and close_led_matrix(),
 * which should be called at the beginning and end of the program using this
 * functionality, respectively.
 *
 * The Sense HAT LED matrix uses the 16-bit RGB565 encoding of colors.
 * In RGB565, the 5 most significant bits encode the red channel, the next
 * 6 bits the green channel and the last 5 bits the blue channel.
 * The function make_rgb565_color() is a convenient way of converting RGB
 * colors in the more common range 0-255 per channel to the RG565 format.
 * Some RGB565 colors are also predefined here.
 *
 * Written by Pontus Ekberg <pontus.ekberg@it.uu.se>
 * Last updated 2019-09-24
 */

#include <stdint.h>

#define LED_MATRIX_FILEPATH "/dev/fb1"
#define NUM_LEDS 64
#define ROW_SIZE 8
#define COL_SIZE 8
#define LED_MATRIX_FILESIZE (NUM_LEDS * sizeof(uint16_t))

/* These are computed using make_rgb565_color() */
#define RGB565_OFF     0x0000
#define RGB565_WHITE   0xFFFF
#define RGB565_RED     0xF800
#define RGB565_GREEN   0x07E0
#define RGB565_BLUE    0x001F
#define RGB565_CYAN    0x07FF
#define RGB565_MAGENTA 0xF81F
#define RGB565_YELLOW  0xFFE0


/*
 * Open the LED matrix framebuffer device and map it into memory.
 *
 * !!! This function needs to be called before any of the other functions
 * that perform operations on the LED matrix (only once at the start) !!!
 *
 * Returns 0 on success and -1 on error.
 */
int open_led_matrix();

/*
 * Unmaps the LED matrix framebuffer from memory and closes the
 * framebuffer file descriptor.
 *
 * !!! This function should be called after all functions that perform
 * operations on the LED matrix (only once at the end) !!!
 *
 * Returns 0 on success and -1 on error.
 */
int close_led_matrix();

/* 
 * Takes r, g, b values in the more common range 0-255 and returns a
 * 16-bit integer encoding the same color (or as close as possible)
 * in the RGB565 format.
 */
uint16_t make_rgb565_color(int r, int g, int b);

/* Set the whole LED matrix to a single RGB565 <color>. */
void set_leds_single_color(uint16_t color);

/* Turn off all the LEDs. */
void clear_leds();

/* 
 * Set the whole LED matrix according to the array <image> of RGB565 colors.
 * The array <image> should have exactly NUM_LEDS elements.
 */
void set_leds_image(uint16_t *image);

/*
 * Set the single LED at <row> and <col> to the RGB565 <color>.
 */
void set_led(int row, int col, uint16_t color);

