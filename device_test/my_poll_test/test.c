#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/input.h>
//#include <sys/poll.h>
#include <poll.h>
#define DEMO_DEV_NAME "/dev/my_poll_device"

int main()
{
	struct pollfd fds[2];
	int ret;

	char buffer0[64];
	char buffer1[64];

	printf("test start\n");

	fds[0].fd = open("/dev/my_poll_device0", O_RDWR| O_NONBLOCK);
	if (fds[0].fd < 0) {
		printf("open device %s%d failed\n", DEMO_DEV_NAME,0);
		return -1;
	}
	fds[0].events = POLLIN;
	fds[0].revents = 0;


	fds[1].fd = open("/dev/my_poll_device1", O_RDWR| O_NONBLOCK);
	if (fds[1].fd < 0) {
		printf("open device %s%d failed\n", DEMO_DEV_NAME,1);
		return -1;
	}
	fds[1].events = POLLIN;
	fds[1].revents = 0;

	while(1){
		ret = poll(fds,2, -1);
		if(ret == -1)
			return -1;

		if (fds[0].revents & POLLIN) {
			ret = read(fds[0].fd, buffer0, 64);
			if (ret < 0) 
				return -1;
			printf("%s%d : %s",DEMO_DEV_NAME, 0, buffer0);
		}
		if (fds[1].revents & POLLIN) {
			ret = read(fds[1].fd, buffer0, 64);
			if (ret < 0) 
				return -1;
			printf("%s%d : %s",DEMO_DEV_NAME, 1, buffer0);
		}


	}



	return 0;
}
