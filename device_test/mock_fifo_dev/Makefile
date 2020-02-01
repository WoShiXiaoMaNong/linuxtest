BASEINCLUDE ?= ~/runninglinux/runninglinuxkernel_4.0


mockfifodev-objs := mock_fifo_dev.o
obj-m := mockfifodev.o

all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) clean;
	rm -f *.ko;
