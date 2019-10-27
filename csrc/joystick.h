/*
 * A tiny library for using the joystick on the Sense HAT
 * add-on board for the Raspberry Pi.
 *
 * Written by Pontus Ekberg <pontus.ekberg@it.uu.se>
 * Last updated: 2018-09-21
 */

#define JOYSTICK_DEV "/dev/input/event0"

#define JOYSTICK_RELEASE	0
#define JOYSTICK_PRESS		1
#define JOYSTICK_HOLD 		2

#define DIRECTION_NORTH	103
#define DIRECTION_SOUTH	108
#define DIRECTION_WEST	105
#define DIRECTION_EAST	106
#define DIRECTION_DOWN	28

/* 
 * The return type of the read_joystick_input() function.
 *
 * The timestamp_s field is the time the event occured in whole seconds.
 *
 * The timestamp_us field gives additional precision as the number of
 * microseconds that also had passed since the last whole second.
 *
 * The direction field contains the direction of the joystick event,
 * as DIRECTION_NORTH, DIRECTION_SOUTH, DIRECTION_WEST, DIRECTION_EAST
 * or DIRECTION_DOWN.
 * 
 * The type field is JOYSTICK_RELEASE, JOYSTICK_PRESS or JOYSTICK_HOLD.
 */
struct js_event {
	long timestamp_s;
	long timestamp_us;
	unsigned short direction;
	unsigned short type;
};


/*
 * Open the joystick device file.
 *
 * !!! This must be called once before the other functions are used !!!
 */
int open_joystick_device();
/*
 * Close the joystick device file.
 *
 * !!! This must be called once at the end of the program !!!
 */
int close_joystick_device();
/*
 * Read one event from the joystick device file. The event is returned
 * as a struct js_event, with the fields set approriately.
 *
 * This function _blocks_ the calling thread until an input event is read.
 */
struct js_event read_joystick_input();

