#makefile for the sample.c

obj-m := sample.o
CURRENT_PATH = /home/wc1229/pdlab/hook
LINUX_KERNEL := $(shell uname -r)
LINUX_KERNEL_PATH := /home/wc1229/kernel/linux-5.9
all:
	$(MAKE) -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules
	rm -rf modules.order Module.symvers .*.cmd *.o *.mod.c .tmp_versions *.unsigned

clean:
	rm -rf modules.order Module.symvers .*.cmd *.o *.mod.c .tmp_versions *.unsigned
