#makefile for the wait.c

obj-m := wait.o
CURRENT_PATH = /home/wc1229/pdlab/wait
LINUX_KERNEL := $(shell uname -r)
#LINUX_KERNEL_PATH := /usr/src/kernels/
LINUX_KERNEL_PATH := /home/wc1229/kernel/linux-5.9
all:
	$(MAKE) -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules

clean:
	rm *.ko
	rm *.o
