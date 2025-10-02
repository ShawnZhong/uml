LINUX_DIR = linux-6.14
ROOTFS = rootfs/img.ext4

BEAR_CMD := $(if $(shell which bear),bear --append --output compile_commands.json --,)

NPROC := $(shell nproc)

.PHONY: all
all: linux rootfs user run

# Build the User Mode Linux kernel
.PHONY: linux
linux:
	$(MAKE) -C ${LINUX_DIR} -j${NPROC} defconfig ARCH=um
	$(MAKE) -C ${LINUX_DIR} -j${NPROC} ARCH=um

# Build the userspace programs
.PHONY: user
user:
	$(BEAR_CMD) $(MAKE) -C user

# Build the kernel module
.PHONY: kmod
kmod:
	$(BEAR_CMD) $(MAKE) -C kmod

# Build the root filesystem
.PHONY: rootfs
rootfs: user kmod
	$(MAKE) -C rootfs

# Run UML
.PHONY: run
run: rootfs
	./${LINUX_DIR}/linux ubd0=${ROOTFS} root=/dev/ubda mem=256M rw

.PHONY: clean
clean:
	$(MAKE) -C user clean
	$(MAKE) -C kmod clean
	$(MAKE) -C rootfs clean
	$(MAKE) -C ${LINUX_DIR} mrproper
