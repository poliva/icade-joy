/*
 *   icade-joy - userspace input driver for ION iCade Core
 *   Copyright 2014 Pau Oliva Fora <pof@eslack.org>
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation; either version 2
 *   of the License, or (at your option) any later version.
 *
 *   based on:
 *    https://github.com/poliva/opengalax
 *
 */

#include "icade-joy.h"

#define DEBUG 0

#define VERSION "0.1"

void usage() {
	printf("icade-joy v%s - (c)2014 Pau Oliva Fora <pof@eslack.org>\n", VERSION);
	printf("Usage: icade-joy [options]\n");
	printf("	-f                   : run in foreground (do not daemonize)\n");
	printf("	-s <icade-device>    : default=/dev/input/js0\n");
	printf("	-u <uinput-device>   : default=/dev/uinput\n");
	exit (1);
}


int main (int argc, char *argv[]) {

	int foreground = 0;
	int opt;
	int i;

	pid_t pid;
	ssize_t res;

	unsigned char buffer[144];

	struct input_event ev[6];
	struct input_event ev_button[22];
	struct input_event ev_sync;

	conf_data conf;
	conf = config_parse();

	while ((opt = getopt(argc, argv, "hfs:u:?")) != EOF) {
		switch (opt) {
			case 'h':
				usage();
				break;
			case 'f':
				foreground=1;
				break;
			case 's':
				sprintf(conf.joystick_device, "%s", optarg);
				break;
			case 'u':
				sprintf(conf.uinput_device, "%s", optarg);
				break;
			default:
				usage();
				break;
		}
	}

	if (!running_as_root()) {
		fprintf(stderr,"this program must be run as root user\n");
		exit (-1);
	}

	printf("icade-joy v%s ", VERSION);
	fflush(stdout);

	if (!foreground) {

		if ((pid = fork()) < 0)
			exit(1);
		else
			if (pid != 0)
			exit(0);

		/* daemon running here */
		setsid();
		if (chdir("/") != 0) 
			die("Could not chdir");
		umask(0);
		printf("forked into background\n");
	} else
		printf("\n");

	/* create pid file */
	if (!create_pid_file())
		exit(-1);

	if (foreground) {
		printf ("\nConfiguration data:\n");
		printf ("\tjoystick_device=%s\n",conf.joystick_device);
		printf ("\tuinput_device=%s\n",conf.uinput_device);
	}

	// Open joystick port
	open_joystick_port(conf.joystick_device);

	// configure uinput
	setup_uinput_dev(conf.uinput_device);

	// handle signals
	signal_installer();

	// input sync signal:
	memset (&ev_sync, 0, sizeof (struct input_event));
	ev_sync.type = EV_SYN;
	ev_sync.code = 0;
	ev_sync.value = 0;

	// button press signals:
	memset (&ev_button, 0, sizeof (ev_button));

	ev_button[0].type = EV_KEY;
	ev_button[0].code = BTN_A;
	ev_button[0].value = 0;
	ev_button[1].type = EV_KEY;
	ev_button[1].code = BTN_A;
	ev_button[1].value = 1;

	ev_button[2].type = EV_KEY;
	ev_button[2].code = BTN_B;
	ev_button[2].value = 0;
	ev_button[3].type = EV_KEY;
	ev_button[3].code = BTN_B;
	ev_button[3].value = 1;

	ev_button[4].type = EV_KEY;
	ev_button[4].code = BTN_X;
	ev_button[4].value = 0;
	ev_button[5].type = EV_KEY;
	ev_button[5].code = BTN_X;
	ev_button[5].value = 1;

	ev_button[6].type = EV_KEY;
	ev_button[6].code = BTN_Y;
	ev_button[6].value = 0;
	ev_button[7].type = EV_KEY;
	ev_button[7].code = BTN_Y;
	ev_button[7].value = 1;

	ev_button[8].type = EV_KEY;
	ev_button[8].code = BTN_TL;
	ev_button[8].value = 0;
	ev_button[9].type = EV_KEY;
	ev_button[9].code = BTN_TL;
	ev_button[9].value = 1;

	ev_button[10].type = EV_KEY;
	ev_button[10].code = BTN_TR;
	ev_button[10].value = 0;
	ev_button[11].type = EV_KEY;
	ev_button[11].code = BTN_TR;
	ev_button[11].value = 1;

	ev_button[12].type = EV_KEY;
	ev_button[12].code = BTN_SELECT;
	ev_button[12].value = 0;
	ev_button[13].type = EV_KEY;
	ev_button[13].code = BTN_SELECT;
	ev_button[13].value = 1;

	ev_button[14].type = EV_KEY;
	ev_button[14].code = BTN_START;
	ev_button[14].value = 0;
	ev_button[15].type = EV_KEY;
	ev_button[15].code = BTN_START;
	ev_button[15].value = 1;

/*
	ev_button[16].type = EV_KEY;
	ev_button[16].code = BTN_MODE;
	ev_button[16].value = 0;
	ev_button[17].type = EV_KEY;
	ev_button[17].code = BTN_MODE;
	ev_button[17].value = 1;

	ev_button[18].type = EV_ABS;
	ev_button[18].code = ABS_RZ;
	ev_button[18].value = 0;
	ev_button[19].type = EV_ABS;
	ev_button[19].code = ABS_RZ;
	ev_button[19].value = 255;

	ev_button[20].type = EV_ABS;
	ev_button[20].code = ABS_Z;
	ev_button[20].value = 0;
	ev_button[21].type = EV_ABS;
	ev_button[21].code = ABS_Z;
	ev_button[21].value = 255;

	ev_button[18].type = EV_KEY;
	ev_button[18].code = BTN_THUMBL;
	ev_button[18].value = 0;
	ev_button[19].type = EV_KEY;
	ev_button[19].code = BTN_THUMBL;
	ev_button[19].value = 1;

	ev_button[20].type = EV_KEY;
	ev_button[20].code = BTN_THUMBR;
	ev_button[20].value = 0;
	ev_button[21].type = EV_KEY;
	ev_button[21].code = BTN_THUMBR;
	ev_button[21].value = 1;
*/

	// load X,Y into input_events
	memset (ev, 0, sizeof (ev));	//resets object

	ev[0].type = EV_ABS;
	ev[0].code = ABS_HAT0X;
	ev[0].value = 0;
	ev[1].type = EV_ABS;
	ev[1].code = ABS_HAT0X;
	ev[1].value = 1;
	ev[2].type = EV_ABS;
	ev[2].code = ABS_HAT0X;
	ev[2].value = -1;

	ev[3].type = EV_ABS;
	ev[3].code = ABS_HAT0Y;
	ev[3].value = 0;
	ev[4].type = EV_ABS;
	ev[4].code = ABS_HAT0Y;
	ev[4].value = 1;
	ev[5].type = EV_ABS;
	ev[5].code = ABS_HAT0Y;
	ev[5].value = -1;



	while (1) {


		memset (buffer, 0, sizeof (buffer));
		res = read (fd_joystick, &buffer, sizeof (buffer));
		if (res < 0)
			die ("error reading from joystick port");

		if (DEBUG || foreground) {
			for (i=0;i<144;i++) {
				#if 0
				if (i==0) fprintf(stderr,"\nPDU:\n");
				if (buffer[i]!=0) fprintf(stderr, "(%d)%.2x ",i, buffer[i]);
				if ((i+1)%16==0) fprintf(stderr,"\n");
				#endif
				fprintf(stderr, "PDU: %.2x\n", buffer[20]);
			}

		}

		// y/down
		if (buffer[20]==0x1b)
			if (write (fd_uinput, &ev[4], sizeof (struct input_event)) < 0) die ("error: write");

		// y/neutral
		if (buffer[20]==0x1d)
			if (write (fd_uinput, &ev[3], sizeof (struct input_event)) < 0) die ("error: write");

		// y/up
		if (buffer[20]==0x1a)
			if (write (fd_uinput, &ev[5], sizeof (struct input_event)) < 0) die ("error: write");

		// y/neutral
		if (buffer[20]==0x08)
			if (write (fd_uinput, &ev[3], sizeof (struct input_event)) < 0) die ("error: write");
		
		// x/right
		if (buffer[20]==0x07)
			if (write (fd_uinput, &ev[1], sizeof (struct input_event)) < 0) die ("error: write");

		// x/neutral
		if (buffer[20]==0x06)
			if (write (fd_uinput, &ev[0], sizeof (struct input_event)) < 0) die ("error: write");

		// x/left
		if (buffer[20]==0x04)
			if (write (fd_uinput, &ev[2], sizeof (struct input_event)) < 0) die ("error: write");

		// x/neutral
		if (buffer[20]==0x14)
			if (write (fd_uinput, &ev[0], sizeof (struct input_event)) < 0) die ("error: write");

		// button 1 press
		if (buffer[20]==0x1c)
			if (write (fd_uinput, &ev_button[1], sizeof (struct input_event)) < 0) die ("error: write");

		// button 1 release
		if (buffer[20]==0x17)
			if (write (fd_uinput, &ev_button[0], sizeof (struct input_event)) < 0) die ("error: write");

		// button 2 press
		if (buffer[20]==0x18)
			if (write (fd_uinput, &ev_button[3], sizeof (struct input_event)) < 0) die ("error: write");

		// button 2 release
		if (buffer[20]==0x09)
			if (write (fd_uinput, &ev_button[2], sizeof (struct input_event)) < 0) die ("error: write");

		// button 3 press
		if (buffer[20]==0x0c)
			if (write (fd_uinput, &ev_button[5], sizeof (struct input_event)) < 0) die ("error: write");

		// button 3 release
		if (buffer[20]==0x10)
			if (write (fd_uinput, &ev_button[4], sizeof (struct input_event)) < 0) die ("error: write");

		// button 4 press
		if (buffer[20]==0x0d)
			if (write (fd_uinput, &ev_button[9], sizeof (struct input_event)) < 0) die ("error: write");

		// button 4 release
		if (buffer[20]==0x11)
			if (write (fd_uinput, &ev_button[8], sizeof (struct input_event)) < 0) die ("error: write");

		// button 5 press
		if (buffer[20]==0x0b)
			if (write (fd_uinput, &ev_button[7], sizeof (struct input_event)) < 0) die ("error: write");

		// button 5 release
		if (buffer[20]==0x015)
			if (write (fd_uinput, &ev_button[6], sizeof (struct input_event)) < 0) die ("error: write");

		// button 6 press
		if (buffer[20]==0x0e)
			if (write (fd_uinput, &ev_button[11], sizeof (struct input_event)) < 0) die ("error: write");

		// button 6 release
		if (buffer[20]==0x13)
			if (write (fd_uinput, &ev_button[10], sizeof (struct input_event)) < 0) die ("error: write");

		// button 7 press
		if (buffer[20]==0x12)
			if (write (fd_uinput, &ev_button[13], sizeof (struct input_event)) < 0) die ("error: write");

		// button 7 release
		if (buffer[20]==0x0a)
			if (write (fd_uinput, &ev_button[12], sizeof (struct input_event)) < 0) die ("error: write");

		// button 8 press
		if (buffer[20]==0x0f)
			if (write (fd_uinput, &ev_button[15], sizeof (struct input_event)) < 0) die ("error: write");

		// button 8 release
		if (buffer[20]==0x19)
			if (write (fd_uinput, &ev_button[14], sizeof (struct input_event)) < 0) die ("error: write");

		// Sync
		if (write (fd_uinput, &ev_sync, sizeof (struct input_event)) < 0) die ("error state");

	}

	if (ioctl (fd_uinput, UI_DEV_DESTROY) < 0)
		die ("error: ioctl");

	close (fd_uinput);

	return 0;
}
