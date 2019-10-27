/*
 * led_example.c - Shows off how to use the functions declared in led_matrix.h
 *
 * Written by Pontus Ekberg <pontus.ekberg@it.uu.se>
 * Last updated 2019-09-24
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include "led_matrix.h"

int rand_range(int min_n, int max_n) {
	return rand() % (max_n + 1 - min_n) + min_n;
}

void sleep_ms(int milliseconds) {
	usleep(1000 * milliseconds);
}

void flash_colors() {
	uint16_t colors[] = {RGB565_WHITE, RGB565_RED, RGB565_GREEN,
	       	RGB565_BLUE, RGB565_CYAN, RGB565_MAGENTA, RGB565_YELLOW};
	for (int i = 0; i < 7; i++) {
		set_leds_single_color(colors[i]);
		sleep_ms(500);
	}
}

void show_gradient() {
	int red_val = 63;
	int blue_val = 255;
	uint16_t color;
	for (int row = 0; row < ROW_SIZE; row++) {
		for (int col = 0; col < COL_SIZE; col++) {
			red_val += 3;
			blue_val -= 3;
			color = make_rgb565_color(red_val, 0, blue_val);
			set_led(row, col, color);
			sleep_ms(50);
		}
	}
}

void show_random_lights() {
	int row, col, red_val, green_val, blue_val;
	uint16_t color;
	for (int step = 0; step < 100; step++) {
		row = rand_range(0, 7);
		col = rand_range(0, 7);
		red_val = rand_range(0, 255);
		green_val = rand_range(0, 255);
		blue_val = rand_range(0, 255);
		color = make_rgb565_color(red_val, green_val, blue_val);
		set_led(row, col, color);
		sleep_ms(80);
	}
}

void do_the_crab() {
	uint16_t W = RGB565_WHITE;
	uint16_t R = RGB565_RED;
	uint16_t G = RGB565_GREEN;
	uint16_t B = RGB565_BLUE;
	uint16_t Y = RGB565_YELLOW;

	uint16_t crab_image1[NUM_LEDS] = 
		{B, B, W, W, B, W, W, B,
		 B, B, 0, W, B, 0, W, B,
		 B, B, B, R, B, B, R, B,
		 B, B, B, R, B, B, R, B,
		 R, R, B, R, R, R, R, R,
		 B, R, R, R, 0, 0, R, R,
		 R, R, Y, R, R, R, R, R,
		 Y, Y, Y, R, Y, R, Y, R};

	uint16_t crab_image2[NUM_LEDS] = 
		{B, B, W, W, B, W, W, B,
		 B, B, 0, W, B, 0, W, B,
		 B, B, B, R, B, B, R, B,
		 B, B, B, R, B, B, R, B,
		 R, R, B, R, R, R, R, R,
		 R, R, R, R, 0, 0, R, R,
		 Y, Y, Y, R, R, R, R, R,
		 Y, Y, Y, R, Y, R, Y, R};

	for (int i = 0; i < 10; i++) {
		set_leds_image(crab_image1);
		sleep_ms(500);
		set_leds_image(crab_image2);
		sleep_ms(500);
	}
}

int main() {
	if (open_led_matrix() == -1) {
		printf("Failed to initialize LED matrix\n");
		return -1;
	}

	/* Seed random number generator */
	srand(time(NULL)); 

	/* Flash the predefined colors one by one */
	printf("Flashing the predefined colors...\n");
	flash_colors();
	sleep_ms(2000);
	clear_leds();

	/* Light up random LEDs with random colors */
	printf("Lighting up random LEDs...\n");
	show_random_lights();
	sleep_ms(2000);
	clear_leds();

	/* Display a blue-red gradient snaking down */
	printf("Displaying a gradient...\n");
	show_gradient();
	sleep_ms(2000);
	clear_leds();

	/* DO THE CRAB! */
	printf("Do the crab!\n");
	do_the_crab();
	sleep_ms(2000);
	clear_leds();

	if (close_led_matrix() == -1) {
		printf("Could not properly close LED matrix\n");
		return -1;
	}

	return 0;
}

