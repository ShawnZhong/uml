User Mode Linux
==============

- Download Linux kernel: `./clone_linux.py`

- Install dependencies: `./install_deps.sh`

- Build Linux kernel: `./make_linux.py [--um] [--debug] [--clean]`

- Prepare root filesystem: `make` (triggered by `./run_*.py` as well)

- Run kernel: `./run_uml.py` or `./run_qemu.py [--debug]`
