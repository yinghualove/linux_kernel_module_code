#mychardev-objs := scull_v2.o
obj-m := dummy.o

#KDIR =/usr/src/linux-headers-$(shell uname -r)
KDIR =/lib/modules/$(shell uname -r)/build
PWD :=$(shell pwd)
all : bin

	$(MAKE) -C $(KDIR) M=$(PWD) modules
	cp -rf $(PWD)/*.ko $(PWD)/bin/
	gcc  dummy_app.c -o dummy_app
	cp -rf $(PWD)/dummy_app $(PWD)/bin/

bin:
	mkdir bin

.PHONY clean:

#	rm -rf *.o *.ko *.mod.* *.symvers *.order *.cmd
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -rf $(PWD)/bin
	rm -rf dummy_app compile_commands.json
#	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean