#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <termios.h>
#include <errno.h>

#include "gamepad.h"
#include "retroid_input.h"

struct termios orig_config;
struct termios uart_config;

void init_uart(int gamepad)
{
	// Check that gamepad port was actually read
	if (gamepad == -1)
	{
		printf("Failed to open uart port\n");
	}

	// 8 bits per character 
	uart_config.c_cflag &= ~CSIZE;
	uart_config.c_cflag |= CS8;

	// Return from read if we receive at least one byte
	uart_config.c_cc[VMIN]  = 1;
	uart_config.c_cc[VTIME] = 0;

	// Get originally tcconfig
	if (tcgetattr(gamepad, &orig_config) != 0)
	{
    	printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
	}

	// Set uart baud rate
	if (cfsetispeed(&uart_config, 0x1002) < 0 || cfsetospeed(&uart_config, 0x1002) < 0)
	{
		printf("Error setting uart config.\n");
	}
	else 
	{
		printf("Success setting uart config.\n");
	}

	// Send uart config
	if (tcsetattr(gamepad, TCSAFLUSH, &uart_config) < 0) 
	{
		printf("Error in tcsetattr.\n");
	}
	else
	{
		printf("Success in tcsetattr.\n");
	}
}

void enable_analog_sticks(int gamepad)
{
	// Reverse Engineered using Ghidra
	long *buffer;
	unsigned char unknown;
	int counter;

	buffer = (long *)calloc(0x100,1);
	*buffer = 0x2e;
	buffer[1] = 1;
	buffer[2] = 2;
	buffer[3] = 0xe;
	buffer[4] = 3;
	buffer[5] = 0;

	unknown = *(unsigned char *)(buffer + 2);
	for (counter = 0; counter < (int)(unknown + 4); counter = counter + 1)
	{
		write(gamepad, (void *)(buffer + counter), 1);
		usleep(100);
	}

	// Close buffer pointer
	free(buffer);
}