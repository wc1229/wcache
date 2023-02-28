#makefile for the test_kobject.c

obj-m := test_kobject.o
CURRENT_PATH = /home/wc1229/pdlab/sysfs/test_kobject
LINUX_KERNEL := $(shell uname -r)
LINUX_KERNEL_PATH := /home/wc1229/kernel/linux-5.9
all:
	$(MAKE) -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules

clean:
	rm *.ko
	rm *.o
