#!/usr/bin/env python3


from scripts import LINUX_DIR, system


def run_gdb():
    args = [
        f"-iex 'set auto-load safe-path {LINUX_DIR}'",
        "-ex 'target remote :1234'",
        f"-ex 'source {LINUX_DIR}/vmlinux-gdb.py'",
    ]
    system(f"gdb {LINUX_DIR}/vmlinux " + " ".join(args))


if __name__ == "__main__":
    run_gdb()
