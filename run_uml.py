#!/usr/bin/env python3

from scripts import system, LINUX_DIR, ROOTFS_IMG, make_rootfs


def run_uml():
    cmd = [
        f"{LINUX_DIR}/linux",
        f"ubd0={ROOTFS_IMG}",
        "root=/dev/ubda",
        "mem=256M",
    ]
    system(" ".join(cmd))


if __name__ == "__main__":
    make_rootfs()
    run_uml()
