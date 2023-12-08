#mychardev-objs := scull_v2.o
obj-m := scull.o
APP_TARGET = scull_app

#KDIR =/usr/src/linux-headers-$(shell uname -r)
KDIR =/lib/modules/$(shell uname -r)/build
PWD :=$(shell pwd)
all : bin

	$(MAKE) -C $(KDIR) M=$(PWD) modules
	cp -rf $(PWD)/*.ko $(PWD)/bin/
	gcc  scull_app.c -o $(APP_TARGET)
	cp -rf $(APP_TARGET) $(PWD)/bin/

bin:
	mkdir bin

.PHONY clean:

#	rm -rf *.o *.ko *.mod.* *.symvers *.order *.cmd
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -rf $(PWD)/bin
	rm -rf $(APP_TARGET) compile_commands.json
	rm -rf .cache .vscode
#	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean