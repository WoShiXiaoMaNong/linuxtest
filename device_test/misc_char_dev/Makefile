BASEINCLUDE ?= ~/runninglinux/runninglinuxkernel_4.0


mymiscdev-objs := my_misc_dev.o
obj-m := mymiscdev.o

all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) clean;
	rm -f *.ko;
