BASEINCLUDE ?= ~/runninglinux/runninglinuxkernel_4.0

ARM-GCC = arm-linux-gnueabi-gcc
mymodule-objs := my_module.o
obj-m := mymodule.o
module-name = mymodule.ko

all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;
	$(ARM-GCC) test.c -o mymoduletest --static

clean:
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) clean;
	rm -f *.ko;
	rm -f mymoduletest
cp:	
	cp $(module-name) $(BASEINCLUDE)/kmodules/
