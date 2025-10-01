#!/usr/bin/env python3

import argparse

from scripts import system, LINUX_DIR, ROOTFS_IMG, make_rootfs


def run_qemu(debug: bool = False):
    cmd = [
        "qemu-system-x86_64",
        "-smp 4",
        "-cpu max",
        "-m 256M",
        f"-kernel {LINUX_DIR}/arch/x86/boot/bzImage",
        '-append "root=/dev/sda rw console=ttyS0 nokaslr"',
        f"-drive file={ROOTFS_IMG},format=raw",
        "-nographic",
        "-accel kvm",
    ]
    if debug:
        cmd += ["-s", "-S"]
    system(" ".join(cmd))


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--debug", action="store_true")
    args = parser.parse_args()
    make_rootfs()
    run_qemu(**vars(args))
