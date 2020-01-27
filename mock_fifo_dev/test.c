#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#define DEMO_DEV_NAME "/dev/my_demo_fifo_dev"

int main()
{
	int fd;
	int ret;

	char testmessage[] = "Test Message for mock device!";

	printf("test start");

	fd = open(DEMO_DEV_NAME, O_RDWR);
	if (fd < 0) {
		printf("open device %s failed\n", DEMO_DEV_NAME);
		return -1;
	}
	
	int len = sizeof(testmessage);
	ret = write(fd, testmessage, len);
	if (ret != len) {
		printf("Can't wirte on device %d, ret=%d", fd, ret);
		return -1;
	}
	
	close(fd);
	

	fd = open(DEMO_DEV_NAME, O_RDWR);
	
	char *read_buf = (char*) malloc(2 * len);
	
	ret = read(fd, read_buf, 2 * len);
	close(fd);
	
	printf("Readed message size is %d, context is %s\n", ret, read_buf);

	printf("test end\n");
	
	return 0;
}
