BASEINCLUDE ?= ~/runninglinux/runninglinuxkernel_4.0

ARM-GCC = arm-linux-gnueabi-gcc
mockkfifononblockdev-objs := mock_kfifo_nonblock_dev.o
obj-m := mockkfifononblockdev.o

all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;
	$(ARM-GCC) test.c -o mockkfifononblocktest --static

clean:
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) clean;
	rm -f *.ko;
	rm -f mockkfifononblocktest
