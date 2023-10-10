#include <stdio.h>
#include <string.h>
#include "hidapi.h"

int main(int argc, char* argv[])
{
	int res;
	unsigned char buf[256];
	hid_device *handle;
	struct hid_device_info *devs, *cur_dev;

	static const int LOGITECH = 0x046d;
	static const int K380 = 0xb342;
    static const char help[64] = "Usage: k380fn <-normal|-special>\n";

	if (argc != 2) {
		printf(help);
		return 1;
	}

	// found using Packet Logger
	memset(buf,0,sizeof(buf));
	buf[0] = 0x10;
	buf[1] = 0xFF;
	buf[2] = 0x0B;
	buf[3] = 0x1E;
	buf[5] = 0x00;
	// 4 is set below
	buf[6] = 0x00;

	if (strcmp(argv[1], "-normal") == 0) {
		buf[4] = 0x00; // f#-keys
	} else if (strcmp(argv[1], "-special") == 0) {
		buf[4] = 0x01; // function keys
	} else {
		printf(help);
		return 1;
	}

	res = hid_init();

	printf("Discovering devices ...\n");
	devs = hid_enumerate(LOGITECH, K380);
	cur_dev = devs;
	while (cur_dev) {
		if (cur_dev->usage == 6) {
			printf("  Product:      %ls\n", cur_dev->product_string);
			handle = hid_open_path(cur_dev->path);

			res = hid_write(handle, buf, 7);
			if (res < 0) {
				printf("Unable to write()\n");
				printf("Error: %ls\n", hid_error(handle));
			} else {
				printf("Settings Changed\n");
			}

			hid_close(handle);
		}

		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);
	printf("Done\n");

	hid_exit();

	return 0;
}
