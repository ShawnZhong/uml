from .consts import LINUX_DIR, PROJ_DIR
from .utils import system


def make_rootfs():
    system(f"make -C {PROJ_DIR} -j$(nproc) rootfs")


def make_kernel(um: bool = False, debug: bool = False):
    arch = "ARCH=um" if um else ""
    config_path = LINUX_DIR / ".config"
    if not config_path.exists():
        system(f"make -C {LINUX_DIR} -j$(nproc) defconfig {arch}")
    if debug:
        system(
            f"cd {LINUX_DIR} && ./scripts/config "
            "--enable CONFIG_DEBUG_INFO_DWARF5 "
            "--disable DEBUG_INFO_REDUCED "
            "--enable DEBUG_INFO_COMPRESSED_NONE "
            "--disable DEBUG_INFO_SPLIT "
        )
    system(f"make -C {LINUX_DIR} -j$(nproc) {arch}")
