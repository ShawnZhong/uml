#!/usr/bin/env python3

import argparse
import logging

from scripts import LINUX_DIR, LINUX_VERSIONS_DIR, system

LINUX_GIT_URL = "https://github.com/torvalds/linux.git"


def clone_master():
    master_dir = LINUX_VERSIONS_DIR / "master"
    if master_dir.exists():
        logging.info(f"Linux master already cloned to {master_dir}")
        return
    system(f"git clone {LINUX_GIT_URL} {master_dir}")


def add_worktree(version: str, current: bool = False):
    worktree_dir = LINUX_VERSIONS_DIR / version
    if worktree_dir.exists():
        logging.info(f"Linux {version} already cloned to {worktree_dir}")
    else:
        system(f"git worktree add -b {version} {worktree_dir}")

    if current:
        LINUX_DIR.symlink_to(worktree_dir)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("versions", nargs="+", type=str, default=["v6.14"])
    args = parser.parse_args()

    clone_master()
    for version in args.versions:
        add_worktree(version, current=version == args.versions[0])
