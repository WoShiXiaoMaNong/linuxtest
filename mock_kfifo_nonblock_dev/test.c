#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#define DEMO_DEV_NAME "/dev/my_demo_kfifo_nonblock_dev"

int main()
{
	int fd;
	int ret;

	char testmessage[] = "Test Message for mock device1111111111111111\
			      1111111111111111111111111111111111111111111111\
			      333333333333333333333333333333333333333\
			      333333333333333333333333333333333333\
			      1111111111111111111111111111111111111111111111\
			      1111111111111111111111111111111111111111111111\
			      1111111111111111111111111111111111111111111111\
			      1111111111111111111111111111111111111111111111\
			      1111111111111111111111111111111111111111111111\
			      1111111111111111111111111111111111111111111111\
			      333333333333333333333333333333333333333\
			      333333333333333333333333333333333333\
			      333333333333333333333333333333333333333\
			      333333333333333333333333333333333333\
			      333333333333333333333333333333333333333\
			      333333333333333333333333333333333333\
			      333333333333333333333333333333333333333\
			      333333333333333333333333333333333333\
			      333333333333333333333333333333333333333\
			      333333333333333333333333333333333333\
			      333333333333333333333333333333333333333\
			      333333333333333333333333333333333333\
			      33333333333333333333333333333333333\
			      !";

	printf("test start\n");

	fd = open(DEMO_DEV_NAME, O_RDWR| O_NONBLOCK);
	if (fd < 0) {
		printf("open device %s failed\n", DEMO_DEV_NAME);
		return -1;
	}




	int len = sizeof(testmessage);
	//try to read message from empty fifo
	char *read_buf = (char*) malloc(2 *len);
	ret = read(fd, read_buf,2 * len);

	printf("Try to read from empty fifo. %d bytes\nmessage is %s \n",ret,read_buf);



	ret = write(fd, testmessage, len);
	if (ret != len) {
		printf("Can't wirte on device %d, ret=%d\n", fd, ret);
		return -1;
	}
	
	

	
	
	ret = read(fd, read_buf, 2 * len);
	close(fd);
	
	printf("Readed message size is %d, context is %s\n", ret, read_buf);

	printf("test end\n");
	
	return 0;
}
