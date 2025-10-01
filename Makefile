BEAR_CMD := $(if $(shell which bear),bear --append --output compile_commands.json --,)

# Build the root filesystem
.PHONY: rootfs
rootfs: user kmod
	$(MAKE) -C rootfs

# Build the userspace programs
.PHONY: user
user:
	$(BEAR_CMD) $(MAKE) -C user

# Build the kernel module
.PHONY: kmod
kmod:
	$(BEAR_CMD) $(MAKE) -C kmod

.PHONY: clean
clean:
	$(MAKE) -C user clean
	$(MAKE) -C kmod clean
	$(MAKE) -C rootfs clean
