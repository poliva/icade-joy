/*
 *   icade-joy - cheat joystick driver
 *   Copyright 2013 Pau Oliva Fora <pof@eslack.org>
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation; either version 2
 *   of the License, or (at your option) any later version.
 *
 */

#define DEFAULT_PID_FILE "/var/run/icade-joy.pid"

#include "icade-joy.h"

int running_as_root (void) {
	uid_t uid, euid;	
	uid = getuid();
	euid = geteuid();
	if (uid != 0)
		return 0;
	if (euid != 0)
		return 0;
	return 1;
}

int configure_uinput (void) {

	if (ioctl (fd_uinput, UI_SET_EVBIT, EV_KEY) < 0)
		die ("error: ioctl");
	if (ioctl (fd_uinput, UI_SET_KEYBIT, BTN_A) < 0)
		die ("error: ioctl");
	if (ioctl (fd_uinput, UI_SET_KEYBIT, BTN_B) < 0)
		die ("error: ioctl");
	if (ioctl (fd_uinput, UI_SET_KEYBIT, BTN_X) < 0)
		die ("error: ioctl");
	if (ioctl (fd_uinput, UI_SET_KEYBIT, BTN_Y) < 0)
		die ("error: ioctl");
	if (ioctl (fd_uinput, UI_SET_KEYBIT, BTN_TL) < 0)
		die ("error: ioctl");
	if (ioctl (fd_uinput, UI_SET_KEYBIT, BTN_TR) < 0)
		die ("error: ioctl");
	if (ioctl (fd_uinput, UI_SET_KEYBIT, BTN_SELECT) < 0)
		die ("error: ioctl");
	if (ioctl (fd_uinput, UI_SET_KEYBIT, BTN_START) < 0)
		die ("error: ioctl");
	if (ioctl (fd_uinput, UI_SET_KEYBIT, BTN_MODE) < 0)
		die ("error: ioctl");
	if (ioctl (fd_uinput, UI_SET_KEYBIT, BTN_THUMBL) < 0)
		die ("error: ioctl");
	if (ioctl (fd_uinput, UI_SET_KEYBIT, BTN_THUMBR) < 0)
		die ("error: ioctl");

	if (ioctl (fd_uinput, UI_SET_EVBIT, EV_ABS) < 0)
		die ("error: ioctl");

	if (ioctl (fd_uinput, UI_SET_ABSBIT, ABS_X) < 0)
		die ("error: ioctl");
	if (ioctl (fd_uinput, UI_SET_ABSBIT, ABS_Y) < 0)
		die ("error: ioctl");
	if (ioctl (fd_uinput, UI_SET_ABSBIT, ABS_Z) < 0)
		die ("error: ioctl");
	if (ioctl (fd_uinput, UI_SET_ABSBIT, ABS_RX) < 0)
		die ("error: ioctl");
	if (ioctl (fd_uinput, UI_SET_ABSBIT, ABS_RY) < 0)
		die ("error: ioctl");
	if (ioctl (fd_uinput, UI_SET_ABSBIT, ABS_RZ) < 0)
		die ("error: ioctl");
	if (ioctl (fd_uinput, UI_SET_ABSBIT, ABS_HAT0X) < 0)
		die ("error: ioctl");
	if (ioctl (fd_uinput, UI_SET_ABSBIT, ABS_HAT0Y) < 0)
		die ("error: ioctl");

	if (ioctl (fd_uinput, UI_SET_EVBIT, EV_FF) < 0) die ("error: ioctl");
	if (ioctl (fd_uinput, UI_SET_FFBIT, FF_RUMBLE) < 0) die ("error: ioctl");
	if (ioctl (fd_uinput, UI_SET_FFBIT, FF_PERIODIC) < 0) die ("error: ioctl");
	if (ioctl (fd_uinput, UI_SET_FFBIT, FF_SQUARE) < 0) die ("error: ioctl");
	if (ioctl (fd_uinput, UI_SET_FFBIT, FF_TRIANGLE) < 0) die ("error: ioctl");
	if (ioctl (fd_uinput, UI_SET_FFBIT, FF_SINE) < 0) die ("error: ioctl");
	if (ioctl (fd_uinput, UI_SET_FFBIT, FF_GAIN) < 0) die ("error: ioctl");

	memset (&uidev, 0, sizeof (uidev));
	snprintf (uidev.name, UINPUT_MAX_NAME_SIZE, "ION iCade Core uinput driver");
	uidev.id.bustype = BUS_I8042;
	uidev.id.vendor = 0xfe0f;
	uidev.id.product = 0xfe0f;
	uidev.id.version = 0xfe0;
	/*
	uidev.absmin[ABS_X] = 0;
	uidev.absmax[ABS_X] = X_AXIS_MAX-1;
	uidev.absmin[ABS_Y] = 0;
	uidev.absmax[ABS_Y] = Y_AXIS_MAX-1;
	*/

	if (write (fd_uinput, &uidev, sizeof (uidev)) < 0)
		die ("error: write");

	if (ioctl (fd_uinput, UI_DEV_CREATE) < 0)
		die ("error: ioctl");

	return 0;
}

int setup_uinput_dev (const char *ui_dev) {
	fd_uinput = open (ui_dev, O_WRONLY | O_NONBLOCK);
	if (fd_uinput < 0) 
		die ("error: uinput");
	return configure_uinput ();
}


int open_joystick_port (const char *fd_device) {
	fd_joystick = open(fd_device, O_RDONLY|O_CLOEXEC );
	if (fd_joystick == -1) {
		perror ("open_port: Unable to open joystick port");
		exit (1);
	}

	return 0;
}

void signal_handler (int sig) {

        (void) sig;

	remove_pid_file();

	if (ioctl (fd_uinput, UI_DEV_DESTROY) < 0)
		die ("error: ioctl");

	close (fd_uinput);
        printf("\nKilled!\n");
        exit(1);
}

void signal_installer (void) {

        signal(SIGINT, signal_handler);
        signal(SIGTERM, signal_handler);
        signal(SIGHUP, signal_handler);
        signal(SIGQUIT, signal_handler);
        signal(SIGCHLD, signal_handler);
        signal(SIGABRT, signal_handler);
}

int file_exists (char *file) {
        struct stat buf;
        if (stat(file, &buf) == 0)
                return 1;
        return 0;
}

char* default_pid_file (void) {
	char* file = malloc(strlen(DEFAULT_PID_FILE)+2);
	sprintf(file, "%s", DEFAULT_PID_FILE);
	return file;
}

int create_pid_file (void) {

	int fd;
	char *pidfile;
	char buf[100];
	ssize_t cnt;
	char* procpid = malloc( sizeof(buf) + 15 );

	pidfile = default_pid_file();

	if (file_exists(pidfile)) {

		// check if /proc/{pid}/cmdline exists and contains icade-joy
		// if it does, means icade-joy is already running, so we exit cowardly
		// if it does not contain icade-joy, then we remove the old pid file and continue

		fd = open(pidfile, O_RDONLY);
		if (fd < 0) {
			fprintf (stderr,"Could not open pid file: %s\n", pidfile);
			return 0;
		}
		cnt=read(fd, buf, sizeof(buf)-1);
		buf[cnt]='\0';
		
		close(fd);

		strcpy(procpid, "");
		strcat(procpid, "/proc/");
		strcat(procpid, buf);
		strcat(procpid, "/cmdline");

		if (file_exists(procpid)) {
			fd = open(procpid, O_RDONLY);
			if (fd < 0) {
				fprintf (stderr,"Could not open file: %s\n", procpid);
				return 0;
			}

			cnt=read(fd, buf, sizeof(buf)-1);
			buf[cnt]='\0';
			
			close(fd);

			if (strstr(buf,"icade-joy") != NULL) {
				fprintf (stderr,"Refusing to start as another instance is already running\n");
				return 0;
			} else {
				if (!remove_pid_file()) 
					return 0;
			}
		}
	}

	fd = open(pidfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0 ) {
		fprintf(stderr,"Could not write pid file: %s\n", pidfile);
		return 0;
	}

	sprintf( buf, "%d", getpid() );
	if (write(fd, buf, strlen(buf)) < 1) {
		perror("Something wrong happening while writing pid file");
		close(fd);
		return 0;
	}
	close(fd);

	free(procpid);

	return 1;
}

int remove_pid_file (void) {

	char *pidfile;

	pidfile = default_pid_file();

	if (!file_exists(pidfile)) {
		fprintf (stderr,"pid file does not exist: %s\n", pidfile);
		return 1;
	}

	if (unlink(pidfile) != 0) {
		fprintf (stderr,"Could not delete pid file: %s\n", pidfile);
		return 0;
	}
	return 1;
}
