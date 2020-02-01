BASEINCLUDE ?= ~/runninglinux/runninglinuxkernel_4.0

ARM-GCC = arm-linux-gnueabi-gcc
mypollmodule-objs := my_poll_module.o
obj-m := mypollmodule.o
module-name = mypollmodule.ko
test-obj-name = mypollmoduletest


all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;
	$(ARM-GCC) test.c -o $(test-obj-name) --static

clean:
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) clean;
	rm -f *.ko;
	rm -f $(test-obj-name)
cp:	
	cp $(module-name)   $(BASEINCLUDE)/kmodules/
	cp $(test-obj-name) $(BASEINCLUDE)/kmodules/
