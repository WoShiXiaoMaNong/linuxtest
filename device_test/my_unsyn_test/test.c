#define _GUN_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#define DEMO_DEV_NAME "/dev/my_unsyn_device0"

int fd;

void my_fasync_func(int signum, siginfo_t *siginfo, void *act)
{
	int ret;
	char buf[64];
	printf("%s enter\n",__func__);
	if (signum == SIGIO) {
		if (siginfo->si_band & POLLIN) {
			printf("FIFO is not empty!\n");
			ret = read(fd, buf, sizeof(buf));
			if (ret != -1) {
				buf[ret] = '\0';
				puts(buf);
			}
		}

		if (siginfo->si_band & POLLOUT) {
			printf("FIFO is not full!\n");
		}
	}
}

int main()
{
	int ret;
	int flag;
	struct sigaction act, oldact;

	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGIO);
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = my_fasync_func; 

	if ( sigaction(SIGIO, &act, &oldact) )
		goto fail;

	fd = open(DEMO_DEV_NAME, O_RDWR);
	if (fd < 0)
		goto fail;
	puts("Open file succeed!\n");
	//设置异步IO所有权
	if ( fcntl(fd, F_SETOWN, getpid()) == -1 )
		goto fail;
	
	puts("set own!\n");
	//设置SIGIO信号
	if (fcntl(fd, F_SETSIG, SIGIO) == -1 )
		goto fail;

	puts("set sig succeed!\n");
	//获取文件flags
	if ( (flag = fcntl(fd, F_GETFL)) == -1 )
		goto fail;

	puts("get flages succeed!\n");
	//设置文件flags, 设置FASYNC，支持异步通知
	if ( fcntl(fd, F_SETFL, flag |  FASYNC) == -1 )
		goto fail;


	puts("set flags  succeed!\n");
	while(1)
		sleep(1);

	fail:
		perror("fasync test error!\n");
		exit(EXIT_FAILURE);

}



