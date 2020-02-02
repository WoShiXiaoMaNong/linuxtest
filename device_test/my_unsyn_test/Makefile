BASEINCLUDE ?= ~/runninglinux/runninglinuxkernel_4.0

ARM-GCC = arm-linux-gnueabi-gcc
myunsynmodule-objs := my_unsyn_module.o
obj-m := myunsynmodule.o
module-name = myunsynmodule.ko
test-obj-name = myunsynmoduletest


all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;
	$(ARM-GCC) test.c  -D _GNU_SOURCE -o $(test-obj-name) --static

clean:
	$(MAKE) -C $(BASEINCLUDE) SUBDIRS=$(PWD) clean;
	rm -f *.ko;
	rm -f $(test-obj-name)
cp:	
	cp $(module-name)   $(BASEINCLUDE)/kmodules/
	cp $(test-obj-name) $(BASEINCLUDE)/kmodules/
