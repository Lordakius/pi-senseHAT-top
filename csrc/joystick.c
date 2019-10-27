/*
 * A tiny library for using the joystick on the Sense HAT
 * add-on board for the Raspberry Pi.
 *
 * Written by Pontus Ekberg <pontus.ekberg@it.uu.se>
 * Last updated: 2018-09-27
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "joystick.h"

/* The format of the data read from the joystick device file. */
struct js_event_raw {
	long timestamp_s;
	long timestamp_us;
	unsigned short action;
	unsigned short direction;
	int type;
};


int jsfd;

/*
 * Open the joystick device file.
 *
 * !!! This must be called once before the other functions are used !!!
 */
int open_joystick_device() {
	jsfd = open(JOYSTICK_DEV, O_RDONLY);
	if (jsfd == -1) {
		perror("Failed to open joystick device. Check user groups");
		return -1;
	}
	return 0;
}

/*
 * Close the joystick device file.
 *
 * !!! This must be called once at the end of the program !!!
 */
int close_joystick_device() {
	if (close(jsfd)) {
		perror("Failed to close joystick device");
		return -1;
	}
	return 0;
}

/*
 * Read one event from the joystick device file. The event is returned
 * as a struct js_event, with the fields set approriately.
 *
 * This function _blocks_ the calling thread until an input event is read.
 */
struct js_event read_joystick_input() {
	struct js_event_raw evr;
	struct js_event ev;

	read(jsfd, &evr, sizeof(evr));
	
	/* Ignore these "extra" events. Read the next one. */
	if (!evr.action) { 
		read(jsfd, &evr, sizeof(evr));
	}

	/* Return a cleaner struct. */
	ev.timestamp_s = evr.timestamp_s;
	ev.timestamp_us = evr.timestamp_us;
	ev.direction = evr.direction;
	ev.type = evr.type;
	return ev;
}

