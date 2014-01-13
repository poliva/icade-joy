/*
 *   icade-joy - userspace input driver for ION iCade Core
 *   Copyright 2014 Pau Oliva Fora <pof@eslack.org>
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation; either version 2
 *   of the License, or (at your option) any later version.
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <linux/uinput.h>
#include <sys/stat.h>

#define die(str, args...) do { \
	perror(str); \
	exit(EXIT_FAILURE); \
} while(0)

/* config file */
typedef struct {
	char joystick_device[1024];
	char uinput_device[1024];
} conf_data;

typedef struct {
	int xmin;
	int xmax;
	int ymin;
	int ymax;
} calibration_data;

int fd_joystick, fd_uinput;
struct uinput_user_dev uidev;

/* configfile.c */
int create_config_file (char* file);
conf_data config_parse (void);
calibration_data calibration_parse (void);

/* functions.c */
int running_as_root (void);
int time_elapsed_ms (struct timeval *start, struct timeval *end, int ms); 
int configure_uinput (void);
int setup_uinput (void);
int setup_uinput_dev (const char *ui_dev);
int open_joystick_port (const char *fd_device); 
void signal_handler (int sig);
void signal_installer (void);
int file_exists (char *file);
char* default_pid_file (void); 
int create_pid_file (void); 
int remove_pid_file (void);
