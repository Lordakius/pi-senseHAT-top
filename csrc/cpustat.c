#define _POSIX_C_SOURCE 199309L

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

#include "joystick.h"
#include "led_matrix.h"


//this is the file to be read to get stats
#define STATFILE "/proc/stat"
#define SAVEFILE ".state"

#define BUFFERSIZE 255
#define MAX_CPU 4 //defines amount of CPUs, maybe take them from system instead?

enum cpuload {ZERO, HALF, FULL};

struct cpustate {
	char name[5];
	//saves state data
	uint64_t user;
	uint64_t nice;
	uint64_t system;
	uint64_t idle;
	uint64_t iowait;
	uint64_t irq;
	uint64_t softirq;
	uint64_t steal;
	//not sure on these, but we dont need them for our calculation
	uint64_t guest;
	uint64_t guest_idle;
	uint64_t prev_idle;
	uint64_t prev_total;
	double perc_use;
};

//doing those globally, easier to prototype
//TODO: use references, make it fancy
struct cpustate *cpulist[MAX_CPU + 1];

static volatile int sig_int = 0;
static volatile enum cpuload curr_load = ZERO;

void int_handler(int sig) {
	if(sig != SIGINT)
		return; //do nothing
	else {
		sig_int = 1; //return after finish of current iteration 
	}
}

void sleep_s(int s) {
	struct timespec req;
	req.tv_sec = s;
	req.tv_nsec = 0;

	nanosleep(&req, NULL);
}

//so gcc doesnt optimize our load stuff
__attribute__((optimize("O0")))
void *handle_input(void *arg) {
	(void)arg;
	struct js_event new_input;
	while(1) {
		sleep_s(1);

		new_input = read_joystick_input();
		if(new_input.type == JOYSTICK_PRESS) {
			switch(new_input.direction) {
				case DIRECTION_WEST:
					curr_load = HALF;
					break;
				case DIRECTION_EAST:
					curr_load = FULL;
					break;
				default:
					break;
			}
			//add new options here

			//this does not work as intended, just use the programs in the load subfolder
			if(curr_load == HALF) {
				fork();
				int a = 1;
			
				for(size_t i = 1; i < 100000000; i++) {
					//just some stuff so the cpu is busy
					a *= i;
				}
			} else if (curr_load == FULL) {
				fork();
				fork();
				int a = 1;
				for(size_t i = 1; i < 100000000; i++) {
					a *= i;
				}
			} else {
				//reset load
				curr_load = ZERO;
			}
		}
	}
}

void printStates(void) {
	clear_leds();
	double perc;
	for(size_t i = 0; i <= MAX_CPU; i++) {
		//DEBUG:
		//printf("%s : %f\n", cpulist[i]->name, cpulist[i]->perc_use);
		
		perc = cpulist[i]->perc_use;

		if(perc > 0)
			set_led(i, 0, RGB565_GREEN);
		if(perc > 12.5)
			set_led(i, 1 , RGB565_GREEN);
		if(perc > 25) 
			set_led(i, 2 , RGB565_GREEN);
		if(perc > 37.5) 
			set_led(i, 3 , RGB565_GREEN);
		if(perc > 50) 
			set_led(i, 4 , RGB565_GREEN);
		if(perc > 62.5) 
			set_led(i, 5 , RGB565_GREEN);
		if(perc > 75) 
			set_led(i, 6 , RGB565_GREEN);
		if(perc > 87.5) 
			set_led(i, 7 , RGB565_GREEN);
	}
}


void getStates(void) {
	char buffer[BUFFERSIZE];

	char *token;

	int run = 1;
	size_t cpu_i = 0;

	struct cpustate *new_state;

	FILE *statfile = fopen(STATFILE, "r");

	while(run) {
		if(!fgets(buffer, BUFFERSIZE, statfile)) {
			perror("file read failed!");	
			exit(EXIT_FAILURE);
		}

		if(strncmp(buffer, "intr", 4)) {
			//intr is the first line after the cpu lines in stat

			new_state = cpulist[cpu_i];

			token = strtok(buffer, " ");
			//first one is always the cpu string
			strncpy(new_state->name, buffer, 4);

			//TODO make this a bit more elegant, for prototype its fine
			if((token = strtok(NULL, " "))) {
				//user
				new_state->user = atoll(token);
			}
			if((token = strtok(NULL, " "))) {
				//nice
				new_state->nice = atoll(token);
			}
			if((token = strtok(NULL, " "))) {
				//system
				new_state->system = atoll(token);
			}
			if((token = strtok(NULL, " "))) {
				//idle
				new_state->idle = atoll(token);
			}
			if((token = strtok(NULL, " "))) {
				//iowait
				new_state->iowait = atoll(token);
			}
			if((token = strtok(NULL, " "))) {
				//irq
				new_state->irq = atoll(token);
			}
			if((token = strtok(NULL, " "))) {
				//softirq
				new_state->softirq = atoll(token);
			}
			if((token = strtok(NULL, " "))) {
				//steal
				new_state->steal= atoll(token);
			}
			
			cpu_i++;
		} else {
			run = 0;
		}

	}
	fclose(statfile);
}

void calcUsage(void) {
	//for math see https://github.com/Leo-G/DevopsWiki/wiki/How-Linux-CPU-Usage-Time-and-Percentage-is-calculated
	
	struct cpustate *new_state;
	uint64_t totaltime, diff_idle, diff_total;

	for(size_t cpu_i = 0; cpu_i <= MAX_CPU; cpu_i++) {
		new_state = cpulist[cpu_i];
		
		totaltime = 
			new_state->user + new_state->nice + new_state->system + new_state->idle + 
			new_state->iowait + new_state->irq + new_state->softirq	+ new_state->steal;

		diff_idle = new_state->idle - new_state->prev_idle;
		diff_total = totaltime - new_state->prev_total;
	
		new_state->perc_use = 
			(double)(1000*(diff_total - diff_idle) / diff_total+5) / 10;

		//remember previous values
		new_state->prev_idle = new_state->idle;
		new_state->prev_total = totaltime;
	}
}

void saveStates(void) {
	//saves states to specified savefile, 
	//TODO: discuss the interface
	//TODO: also discuss if we should overwrite (as we do now) or append it --
	//will probably need a bit more work, but also needed for history
	
	//b-mode added for compability with non-UNIX systems
	FILE *savefile = fopen(SAVEFILE, "ab");
	if(!savefile) {
		perror("savefile");
		return;
	}
	fwrite(cpulist[0], sizeof(struct cpustate), MAX_CPU+1, savefile);
	//TODO: test if this is needed for it to be readable?
	//	fputs("\n", savefile);
}

int main(void) {
	//TODO add help/info/startup options etc.

	for(size_t i = 0; i <= MAX_CPU; i++) {
		cpulist[i] = malloc(sizeof(struct cpustate));
		
		if(cpulist[i] == NULL) {
			perror("calloc");
			return(EXIT_FAILURE);
		}
	}

	pthread_t inputthread;

	if(pthread_create(&inputthread, NULL, handle_input, NULL)) {
		perror("thread creation failed");
		return EXIT_FAILURE;
	}
	if(open_led_matrix() == -1) {
		perror("matrix init");
		exit(EXIT_FAILURE);
	}

	signal(SIGINT, int_handler);
	while(!sig_int) {
		getStates();
		//saveStates(); //procures strange bugs and lots of (unecessary) work
		calcUsage();
		printStates();
		sleep_s(1);

	}

	clear_leds();

	if(close_led_matrix() == -1) {
		perror("matrix close");
		exit(EXIT_FAILURE);
	}
	for(size_t i = 0;i <= MAX_CPU; i++) {
		free(cpulist[i]);
	}
}
