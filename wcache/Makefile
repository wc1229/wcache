#makefile for the wcache.c

obj-m := cache.o
cache-objs := wcache.o sysfs.o rb_tree.o test.o

CURRENT_PATH = /home/wc1229/pdlab/wcache
LINUX_KERNEL := $(shell uname -r)
LINUX_KERNEL_PATH := /home/wc1229/kernel/linux-5.9
all:
	$(MAKE) -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules
	rm -rf modules.order Module.symvers .*.cmd *.o *.mod.c .tmp_versions *.unsigned

clean:
	rm -rf modules.order Module.symvers .*.cmd *.o *.mod.c .tmp_versions *.unsigned
