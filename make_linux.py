#!/usr/bin/env python3

import argparse

from scripts import LINUX_DIR, system


def make_linux(um: bool = False, debug: bool = False, clean: bool = False):
    # Clean up old build
    if clean:
        system(f"make -C {LINUX_DIR} -j$(nproc) mrproper")

    # Build for UML
    if um:
        system(f"make -C {LINUX_DIR} -j$(nproc) ARCH=um defconfig")
        system(f"make -C {LINUX_DIR} -j$(nproc) ARCH=um")
        return

    # Generate config
    if not (LINUX_DIR / ".config").exists():
        system(f"make -C {LINUX_DIR} -j$(nproc) defconfig")

    # Enable debug symbols
    if debug:
        args = [
            "--enable DEBUG_INFO_DWARF5",
            "--disable DEBUG_INFO_REDUCED",
            "--enable DEBUG_INFO_COMPRESSED_NONE",
            "--disable DEBUG_INFO_SPLIT",
            "--enable GDB_SCRIPTS",
        ]
        system(f"cd {LINUX_DIR} && ./scripts/config " + " ".join(args))

    # Build kernel
    system(f"make -C {LINUX_DIR} -j$(nproc)")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--um", action=argparse.BooleanOptionalAction, default=False)
    parser.add_argument("--debug", action=argparse.BooleanOptionalAction, default=True)
    parser.add_argument("--clean", action=argparse.BooleanOptionalAction, default=False)
    args = parser.parse_args()
    make_linux(**vars(args))
