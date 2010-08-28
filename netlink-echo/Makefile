obj-m += netlink-echo-kernel.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc -g -o netlink-echo-userspace -Wall netlink-echo-userspace.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm netlink-echo-userspace

