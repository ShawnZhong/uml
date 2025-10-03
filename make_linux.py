#!/usr/bin/env python3

import argparse

from scripts import LINUX_DIR, system


def make_linux(uml: bool = False, debug: bool = False, clean: bool = False):
    extra = " ARCH=um" if uml else ""

    # Clean up old build
    if clean:
        system(f"make -C {LINUX_DIR} -j$(nproc) mrproper")

    # Generate config
    if not (LINUX_DIR / ".config").exists():
        system(f"make -C {LINUX_DIR} -j$(nproc) defconfig {extra}")

    # Enable debug symbols
    if debug:
        args = [
            "--enable KPROBES",
            "--enable DEBUG_INFO_DWARF5",
            "--disable DEBUG_INFO_REDUCED",
            "--enable DEBUG_INFO_COMPRESSED_NONE",
            "--disable DEBUG_INFO_SPLIT",
            "--enable GDB_SCRIPTS",
        ]
        system(f"cd {LINUX_DIR} && ./scripts/config " + " ".join(args))

    # Build kernel
    system(f"make -C {LINUX_DIR} -j$(nproc) {extra}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--uml", action=argparse.BooleanOptionalAction, default=False)
    parser.add_argument("--debug", action=argparse.BooleanOptionalAction, default=True)
    parser.add_argument("--clean", action=argparse.BooleanOptionalAction, default=False)
    args = parser.parse_args()
    make_linux(**vars(args))
