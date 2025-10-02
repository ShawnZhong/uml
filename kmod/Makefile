OUT ?= $(abspath ../rootfs/data)
SRC ?= $(shell pwd)
KERNELDIR ?= $(abspath ../linux/current)

obj-m := sched_sim.o sched_trace.o

# Allow the use of private headers
ccflags-y := -I$(KERNELDIR) -Wno-unused-variable -Wno-unused-function

sched_sim.ko sched_trace.ko: sched_sim.c sched_trace.c kernel_sym.h utils.h logging.h
	$(MAKE) -C ${KERNELDIR} M=$(SRC) modules
	cp $(SRC)/*.ko $(OUT)

.PHONY: clean
clean:
	$(MAKE) -C ${KERNELDIR} M=$(SRC) clean
