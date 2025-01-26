obj-m += ZynqRPNDriver.o

KERNEL_DIR=~/PynqLinuxZynq/linux-xlnx-xlnx_rebase_v5.15_LTS_2022.1_update2

all:
	ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- make -C $(KERNEL_DIR) M=$(shell pwd) modules

clean:
	make -C $(KERNEL_DIR) M=$(shell pwd) clean 



