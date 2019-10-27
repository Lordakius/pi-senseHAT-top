CC = gcc
CFLAGS= -std=c11 -Wall -Wextra -pedantic -g -O3 -pthread

#add target programs here
TARGETS= cpustat
	
all: $(TARGETS)

$(TARGETS): led_matrix.o joystick.o

clean :
	rm cpustat

.PHONY: all clean
