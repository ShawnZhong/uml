#!/usr/bin/env python3

import argparse

from scripts import LINUX_DIR, system


def make_linux(um: bool = False, debug: bool = False, clean: bool = False):
    arch = "ARCH=um" if um else ""

    # Clean up old build
    if clean:
        system(f"make -C {LINUX_DIR} -j$(nproc) mrproper")

    # Generate config
    if not (LINUX_DIR / ".config").exists():
        system(f"make -C {LINUX_DIR} -j$(nproc) defconfig {arch}")

    # Enable debug symbols
    if debug:
        system(
            f"cd {LINUX_DIR} && ./scripts/config "
            "--enable CONFIG_DEBUG_INFO_DWARF5 "
            "--disable DEBUG_INFO_REDUCED "
            "--enable DEBUG_INFO_COMPRESSED_NONE "
            "--disable DEBUG_INFO_SPLIT "
        )

    # Build kernel
    system(f"make -C {LINUX_DIR} -j$(nproc) {arch}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--um", action=argparse.BooleanOptionalAction, default=False)
    parser.add_argument("--debug", action=argparse.BooleanOptionalAction, default=True)
    parser.add_argument("--clean", action=argparse.BooleanOptionalAction, default=False)
    args = parser.parse_args()
    make_linux(**vars(args))
