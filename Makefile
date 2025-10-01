LINUX_DIR = linux-6.14

BEAR_CMD := $(if $(shell which bear),bear --append --output compile_commands.json --,)

.PHONY: all
all: linux rootfs user

# Build the User Mode Linux kernel
.PHONY: uml
uml:
	$(MAKE) -C ${LINUX_DIR} -j$(nproc) defconfig ARCH=um
	$(MAKE) -C ${LINUX_DIR} -j$(nproc) ARCH=um

.PHONY: linux
linux:
	$(MAKE) -C ${LINUX_DIR} -j$(nproc) defconfig
	$(MAKE) -C ${LINUX_DIR} -j$(nproc)

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

.PHONY: clean
clean:
	$(MAKE) -C user clean
	$(MAKE) -C kmod clean
	$(MAKE) -C rootfs clean
	$(MAKE) -C ${LINUX_DIR} mrproper
