CROSS := 1

PWD = $(shell pwd)
TARGET1 = gpio_lkm
TARGET2 = bbb-gpio
TARGET3 = chardev
TARGET4 = ./module/my_driver


CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build


obj-m := $(TARGET1).o $(TARGET2).o $(TARGET3).o $(TARGET4).o

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	@rm -f *.o *.cmd *.flags *.mod.c *.order
	@rm -f .*.*.cmd *~ *.*~ TODO.*
	@rm -fR .tmp*

disclean: clean
	@rm *.ko *.symvers