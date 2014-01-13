#include "icade-joy.h"

#define CONFIG_FILE "/etc/icade-joy.conf"
#define MAXLEN 1024

static const conf_data default_config = {
	/* joystick_device */ "/dev/input/js0",
	/* uinput_device */ "/dev/uinput",
};

int create_config_file (char* file) {
        FILE* fd;

        fd = fopen(file, "w");
        if (fd == NULL)
                return 0;

        fprintf(fd, "# icade-joy configuration file\n");
        fprintf(fd, "joystick_device=%s\n", default_config.joystick_device);
        fprintf(fd, "uinput_device=%s\n", default_config.uinput_device);
        fprintf(fd, "\n");


        fclose(fd);
        return 1;
}

conf_data config_parse (void) {

	char file[MAXLEN];
	char input[MAXLEN], temp[MAXLEN];
	FILE *fd;
	size_t len;
	conf_data config = default_config;

	sprintf( file, "%s", CONFIG_FILE);
	if (!file_exists(file)) {
		if (!create_config_file(file)) {
			fprintf (stderr,"Failed to create default config file: %s\n", file);
			exit (1);
		}
	}

	fd = fopen (file, "r");
	if (fd == NULL) {
		fprintf (stderr,"Could not open configuration file: %s\n", file);
		exit (1);
	}

	while ((fgets (input, sizeof (input), fd)) != NULL) {

		if ((strncmp ("joystick_device=", input, 16)) == 0) {
			strncpy (temp, input + 16,MAXLEN-1);
			len=strlen(temp);
			temp[len-1]='\0';
			sprintf(config.joystick_device, "%s", temp);
			memset (temp, 0, sizeof (temp));
		}

		if ((strncmp ("uinput_device=", input, 14)) == 0) {
			strncpy (temp, input + 14,MAXLEN-1);
			len=strlen(temp);
			temp[len-1]='\0';
			sprintf(config.uinput_device, "%s", temp);
			memset (temp, 0, sizeof (temp));
		}
	}

	fclose(fd);
	return config;
}
