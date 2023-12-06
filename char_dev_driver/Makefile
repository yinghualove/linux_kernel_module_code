mychardev-objs := char_dev.o
obj-m := mychardev.o

#KDIR =/usr/src/linux-headers-$(shell uname -r)
KDIR =/lib/modules/$(shell uname -r)/build
all : bin

	$(MAKE) -C $(KDIR) M=$(PWD) modules
	cp -rf $(PWD)/*.ko $(PWD)/bin/

bin:
	mkdir bin

.PHONY clean:

#	rm -rf *.o *.ko *.mod.* *.symvers *.order *.cmd
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -rf $(PWD)/bin
#	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean