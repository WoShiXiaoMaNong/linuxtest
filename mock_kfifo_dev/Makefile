BASEINCLUDE ?= ~/runninglinux/runninglinuxkernel_4.0


mockkfifodev-objs := mock_kfifo_dev.o
obj-m := mockkfifodev.o

all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) clean;
	rm -f *.ko;
