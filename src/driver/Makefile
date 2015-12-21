ifneq ($(KERNELRELEASE),)
	obj-m:=fcprobe_main.o
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD :=$(shell pwd)
	
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

clean :
	rm -f *.o *.ko *.mod.c *.unsigned *.symvers *.mod.o *.order *.*~
