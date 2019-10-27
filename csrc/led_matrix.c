/*
 * led_matrix.c
 *
 * This file contains convenient functions for activating the LED matrix
 * on the Raspberry Pi Sense HAT (astropi).
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
 * Some RGB565 colors are also predefined in led_matrix.h.
 *
 * Written by Pontus Ekberg <pontus.ekberg@it.uu.se>
 * Last updated 2019-09-24
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <linux/input.h>

#include "led_matrix.h"

int fbfd;
uint16_t *led_map;

/* 
 * Open the LED matrix framebuffer device (which is a special file in /dev).
 *
 * Returns the file descriptor if successful or -1 on error.
 */
static int open_led_fb() {

	int fbfd;
	struct fb_fix_screeninfo info;
	
	/* Open the LED matrix framebuffer device */
	fbfd = open(LED_MATRIX_FILEPATH, O_RDWR);
	if (fbfd == -1) {
		perror("Error on call to open()");
		return -1;
	}

	/* Read fixed screen info for the open device */
	if (ioctl(fbfd, FBIOGET_FSCREENINFO, &info) == -1) {
		perror("Error on call to ioctl()");
		close(fbfd);
		return -1;
	}

	/* Check that the correct device has been found */
	if (strcmp(info.id, "RPi-Sense FB") != 0) {
		printf("Wrong device found\n");
		close(fbfd);
		return -1;
	}

	return fbfd;
}

/*
 * Map the framebuffer device (opened with file descriptor fbfd) into
 * memory using mmap. 
 *
 * Returns a pointer to the beginning of the mapped memory area if
 * successful, or NULL on error.
 */
static uint16_t *mmap_led_fb(int fbfd) {

	uint16_t *led_map;
	
	/* Map the LED matrix framebuffer device into memory */
	led_map = mmap(NULL, LED_MATRIX_FILESIZE, PROT_READ | PROT_WRITE,
		       	MAP_SHARED, fbfd, 0);
	if (led_map == MAP_FAILED) {
		perror("Error on call to mmap()");
		close(fbfd);
		return NULL;
	}

	return led_map;
}

/*
 * Open the LED matrix framebuffer device and map it into memory.
 *
 * !!! This function needs to be called before any of the other functions
 * that perform operations on the LED matrix (only once at the start) !!!
 *
 * Returns 0 on success and -1 on error.
 */
int open_led_matrix() {

	fbfd = open_led_fb();
	if (fbfd == -1) {
		return -1;
	}

	led_map = mmap_led_fb(fbfd);
	if (led_map == NULL) {
		return -1;
	}

	return 0;
}

/*
 * Unmaps the LED matrix framebuffer from memory and closes the
 * framebuffer file descriptor.
 *
 * !!! This function should be called after all functions that perform
 * operations on the LED matrix (only once at the end) !!!
 *
 * Returns 0 on success and -1 on error.
 */
int close_led_matrix() {

	int ret = 0;
	if (munmap(led_map, LED_MATRIX_FILESIZE) == -1) {
		perror("Error on call to munmap()");
		ret = -1;
	}
	if (close(fbfd) == -1) {
		perror("Error on call to close()");
		ret = -1;
	}
	return ret;
}

/* 
 * Takes r, g, b values in the more common range 0-255 and returns a
 * 16-bit integer encoding the same color (or as close as possible)
 * in the RGB565 format.
 */
uint16_t make_rgb565_color(int r, int g, int b) {
	
	r = (r >> 3) & 0x1F;
	g = (g >> 2) & 0x3F;
	b = (b >> 3) & 0x1F;
	int rgb565 = (r << 11) + (g << 5) + b;
	return rgb565;
}

/* Set the whole LED matrix to a single RGB565 <color>. */
void set_leds_single_color(uint16_t color) {
	
	uint16_t *p = led_map;
	for (int i = 0; i < NUM_LEDS; i++) {
		*(p + i) = color;
	}
}

/* Turn off all the LEDs. */
void clear_leds() {
	
	set_leds_single_color(RGB565_OFF);
}

/* 
 * Set the whole LED matrix according to the array <image> of RGB565 colors.
 * The array <image> should have exactly NUM_LEDS elements.
 */
void set_leds_image(uint16_t *image) {

	uint16_t *p = led_map;
	for (int i = 0; i < NUM_LEDS; i++) {
		*(p + i) = image[i];
	}
}

/*
 * Set the single LED at <row> and <col> to the RGB565 <color>.
 */
void set_led(int row, int col, uint16_t color) {

	int led_num = row * ROW_SIZE + col;
	if (led_num >= NUM_LEDS) {
		printf("LED (%d, %d) does not exist!\n", row, col);
		return;
	}
	*(led_map + led_num) = color;
}

