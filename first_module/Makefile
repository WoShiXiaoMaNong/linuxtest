BASEINCLUDE ?= /lib/modules/5.3.0-28-generic/build


mytest-objs := my_test.o
obj-m := mytest.o

all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) clean;
	rm -f *.ko;
