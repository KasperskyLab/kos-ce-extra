# Using the KasperskyOS-adapted CPython to run third-party Python packages

This solution demonstrates how to use external Python packages with the
[KasperskyOS-adapted version of Python interpreter](https://kas.pr/r46m)
using Numpy as an example.

## Table of contents
- [Using the KasperskyOS-adapted CPython to run third-party Python packages](#using-the-kasperskyos-adapted-cpython-to-run-third-party-python-packages)
  - [Table of contents](#table-of-contents)
  - [Solution overview](#solution-overview)
    - [List of programs](#list-of-programs)
    - [Initialization description](#initialization-description)
    - [Security policy description](#security-policy-description)
  - [Getting started](#getting-started)
    - [Prerequisites](#prerequisites)
    - [Building and running the example](#building-and-running-the-example)
      - [QEMU](#qemu)
      - [Hardware](#hardware)
      - [CMake input files](#cmake-input-files)
  - [Additional files](#additional-files)
  - [Usage](#usage)

## Solution overview

### List of programs

* `Python3`—Python interpreter.
* `VfsSdCardFs`—System program that supports the file system of SD cards.
* `DCM`—System program for dynamic creation of IPC channels.
* `BlobContainer`—System program required for working with dynamic libraries in shared memory.
* `SDCard`—SD card driver.
* `EntropyEntity`—System program that implements random number generation.
* `BSP`—Hardware platform support package (Board Support Package).

When you build the example for the target hardware platform, platform-specific drivers are
automatically included in the solution:

* `GPIO`—GPIO support driver for the Radxa ROCK 3A.
* `PinCtrl`—Low-level pin multiplexing (pinmux) configuration driver for the Radxa ROCK 3A.
* `Bcm2711MboxArmToVc`—Driver for working with the VideoCore (VC6) coprocessor via mailbox
technology for Raspberry Pi 4 B.

### Initialization description

The solution initialization description file named `init.yaml` is generated during the solution
build process based on the [`./einit/src/init.yaml.in`](einit/src/init.yaml.in) template. Macros in
the `@INIT_*@`‌ ‌format contained in the template are automatically expanded in the resulting
`init.yaml` file. For more details, refer to
[init.yaml.in template](https://kas.pr/nb5m).

### Security policy description

The [`./einit/src/security.psl`](einit/src/security.psl) file describes the security policy of the
solution. The declarations in the PSL file are provided with comments that explain the purpose of
these declarations. For more information about the `security.psl` file, see
[Describing a security policy for a KasperskyOS-based solution](https://kas.pr/jm1v).

[⬆ Back to Top](#table-of-contents)

## Getting started

### Prerequisites

1. Confirm that your host system meets all the
[System requirements](https://kas.pr/u87d)
listed in the KasperskyOS Community Edition Developer's Guide.
1. [Install](https://kas.pr/ce73)
the KasperskyOS Community Edition SDK version 1.4. You can download it for free from
[os.kaspersky.com](https://kas.pr/4mro).
1. Copy the source files of this example to your local project directory.
1. Source the SDK setup script to configure the build environment. This exports the `KOSCEDIR`
  environment variable, which points to the SDK installation directory:
   ```sh
   source /opt/KasperskyOS-Community-Edition-<platform>-<version>/common/set_env.sh
   ```
1. [Build the necessary drivers](https://kas.pr/ia13)
from source only if you intend to run this example on Radxa ROCK 3A hardware. This step is not
required for QEMU or Raspberry Pi 4 B.
1. If not already installed, [build and install](https://kas.pr/r46m) the
KasperskyOS-adapted Python, as it is a required dependency for this project.

### Building and running the example

The example is built using the CMake build system, which is provided in the KasperskyOS Community
Edition SDK. When you develop a KasperskyOS-based solution, use the
[recommended structure of project directories](https://kas.pr/9zph)
to simplify the use of CMake scripts.

#### QEMU

To build the example to run on QEMU, go to the directory with the example and run the following
commands:
```sh
cmake -B build \
      -D CMAKE_TOOLCHAIN_FILE=$KOSCEDIR/toolchain/share/toolchain-aarch64-kos.cmake \
      -D CMAKE_FIND_ROOT_PATH=~/.local/share/kos/$(basename $KOSCEDIR)/sysroot-aarch64-kos \
      -D CMAKE_SYSTEM_PREFIX_PATH=~/.local/share/kos/$(basename $KOSCEDIR)/toolchain
cmake --build build --target {kos-qemu-image|sim}
```
where:

* `kos-qemu-image` creates a KasperskyOS-based solution image for QEMU that includes the example;
* `sim` creates a KasperskyOS-based solution image for QEMU that includes the example and runs it.

> [!NOTE]
> * `CMAKE_SYSTEM_PREFIX_PATH` must point to the installation directory of the
>  [Python for build machine](https://kas.pr/r46m#python-for-build-machine).
>  For compatibility with the KasperskyOS adaptation patch, we recommend setting it to
>  `~/.local/share/kos/$(basename $KOSCEDIR)/toolchain`.
> * `CMAKE_FIND_ROOT_PATH` must point to the installation directory of the
>  [Python for KasperskyOS](https://kas.pr/r46m#building-python-for-kasperskyos).
>  For compatibility, we recommend setting it to
>  `~/.local/share/kos/$(basename $KOSCEDIR)/sysroot-aarch64-kos`.

After a successful build, the `kos-qemu-image` solution image will be located in the `./build/einit`
directory.

#### Hardware

To build the example to run on the target hardware platform, go to the directory with the example
and run the following commands:
```sh
cmake -B build \
      -D CMAKE_TOOLCHAIN_FILE=$KOSCEDIR/toolchain/share/toolchain-aarch64-kos.cmake \
      -D CMAKE_FIND_ROOT_PATH=~/.local/share/kos/$(basename $KOSCEDIR)/sysroot-aarch64-kos \
      -D CMAKE_SYSTEM_PREFIX_PATH=~/.local/share/kos/$(basename $KOSCEDIR)/toolchain
cmake --build build --target {kos-image|sd-image}
```
where:

* `kos-image` creates a KasperskyOS-based solution image that includes the example;
* `sd-image` creates a file system image for a bootable SD card.

> [!NOTE]
> * `CMAKE_SYSTEM_PREFIX_PATH` must point to the installation directory of the
>  [Python for build machine](https://kas.pr/r46m#python-for-build-machine).
>  For compatibility with the KasperskyOS adaptation patch, we recommend setting it to
>  `~/.local/share/kos/$(basename $KOSCEDIR)/toolchain`.
> * `CMAKE_FIND_ROOT_PATH` must point to the installation directory of the
>  [Python for KasperskyOS](https://kas.pr/r46m#building-python-for-kasperskyos).
>  For compatibility, we recommend setting it to
>  `~/.local/share/kos/$(basename $KOSCEDIR)/sysroot-aarch64-kos`.

After a successful build, the `kos-image` solution image will be located in the `./build/einit`
directory. The `hdd.img` bootable SD card image will be located in the `./build` directory.

To run the example on the target hardware platform:

1. Connect the SD card to the computer.
1. Copy the bootable SD card image to the SD card using the command:
    ```sh
    sudo dd bs=64k if=build/hdd.img of=/dev/sd[X] conv=fsync
    ```
    where `[X]` is the final character in the name of the SD card block device.
1. Connect the bootable SD card to the hardware.
1. Supply power to the hardware and wait for the example to run.

You can also use an alternative option to prepare and run the example:

1. Prepare the required hardware platform and a bootable SD card to run the example by following the
instructions:

   - [Raspberry Pi 4 B](https://kas.pr/i8mg).
   - [Radxa ROCK 3A](https://kas.pr/21n7).
1. Run the example by following the instructions in the
[KasperskyOS Community Edition Online Help](https://kas.pr/9oa7).

> [!NOTE]
> If you are building a `kos-image` target for copying to a prepared bootable SD card, ensure that
> the second partition of the bootable SD card is large enough (larger than `./build/hdd/part2`). We
> recommend preparing the bootable SD card for this example using the `sd-image` build target.

[⬆ Back to Top](#table-of-contents)

#### CMake input files

[./numpy/CMakeLists.txt](numpy/CMakeLists.txt)—CMake commands for building and installing the
NumPy Python package.

[./einit/CMakeLists.txt](einit/CMakeLists.txt)—CMake commands for building the `Einit` program and
the solution image.

[./CMakeLists.txt](CMakeLists.txt)—CMake commands for building the solution.

## Additional files

The example contains a few auxiliary files that are required for building the third‑party Python
package (NumPy) for the KasperskyOS target.

- [`./build_requirements.txt`](build_requirements.txt)–list of host‑side Python packages
  (`crossenv`, `unearth`) that are installed before creating the cross‑environment. The file is
  referenced from the top‑level [./CMakeLists.txt](CMakeLists.txt) (see the `crossenv` custom
  target).

- [`./numpy/aarch64-kos-cross-file.ini`](numpy/aarch64-kos-cross-file.ini)–Meson cross‑compilation
  description for the `aarch64‑kos` toolchain. It is copied to the build directory and passed to
  `pip wheel` via the `--cross-file` option when building NumPy (see the
  [./numpy/CMakeLists.txt](numpy/CMakeLists.txt)).

- [`./numpy/patches/aarch64-kos-numpy-2.2.6.patch`](numpy/patches/aarch64-kos-numpy-2.2.6.patch)–
  patch that adjusts NumPy 2.2.6 sources for KasperskyOS (e.g., fixes the `backtrace` signature).
  The patch is applied automatically by the `ExternalProject_Add` step in the NumPy
  sub‑project (see the [./numpy/CMakeLists.txt](numpy/CMakeLists.txt)).

These files are not part of the NumPy upstream distribution; they are required only to successfully
cross‑compile and run NumPy on KasperskyOS. No additional actions are needed from the user–the CMake
workflow takes care of applying the patch, configuring the cross‑file and installing the host‑side
requirements.

## Usage

[Build and run](#building-and-running-the-example) the example. After running the example, the
message `[14 32]` should be displayed in the standard output. This demonstrates that NumPy is
running successfully on KasperskyOS.

[⬆ Back to Top](#table-of-contents)

© 2026 AO Kaspersky Lab
