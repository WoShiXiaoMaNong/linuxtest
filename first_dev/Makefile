BASEINCLUDE ?= ~/runninglinux/runninglinuxkernel_4.0


mydev-objs := my_dev.o
obj-m := mydev.o

all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) clean;
	rm -f *.ko;
