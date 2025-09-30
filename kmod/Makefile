OUT ?= $(abspath ../rootfs/data/kmod)
SRC ?= $(shell pwd)
KERNELDIR ?= $(abspath ../linux-6.14)

obj-m := sched_sim.o sched_trace.o

# Allow the use of private headers
ccflags-y := -I$(KERNELDIR)

sched_sim.ko sched_trace.ko: sched_sim.c sched_trace.c
	$(MAKE) -C ${KERNELDIR} M=$(SRC) modules ARCH=um
	cp $(SRC)/*.ko $(OUT)

.PHONY: clean
clean:
	$(MAKE) -C ${KERNELDIR} M=$(SRC) clean
