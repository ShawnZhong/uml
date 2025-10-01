from pathlib import Path

PROJ_DIR = Path(__file__).parent.parent.resolve()

LINUX_DIR = PROJ_DIR / "linux-6.14"
USER_DIR = PROJ_DIR / "user"
KMOD_DIR = PROJ_DIR / "kmod"
ROOTFS_DIR = PROJ_DIR / "rootfs"
ROOTFS_IMG = ROOTFS_DIR / "img.ext4"
