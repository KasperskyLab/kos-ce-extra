# Using the KasperskyOS-adapted CPython to run a simple script

This solution demonstrates how to integrate the
[KasperskyOS-adapted version of Python interpreter](https://kas.pr/r46m)
into a KasperskyOS-based solution.

## Table of contents
- [Using the KasperskyOS-adapted CPython to run a simple script](#using-the-kasperskyos-adapted-cpython-to-run-a-simple-script)
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
the `@INIT_*@`‌ format contained in the template are automatically expanded in the resulting
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
1. If not already installed, [build and install](https://kas.pr/r46m)
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
      -D CMAKE_FIND_ROOT_PATH=~/.local/share/kos/$(basename $KOSCEDIR)/sysroot-aarch64-kos
cmake --build build --target {kos-qemu-image|sim} # choose one: kos-qemu-image or sim
```
Where:

* `kos-qemu-image` creates a KasperskyOS-based solution image for QEMU that includes the example;
* `sim` creates a KasperskyOS-based solution image for QEMU that includes the example and runs it.

> [!NOTE]
>  Ensure that `CMAKE_FIND_ROOT_PATH` points to where you installed the
>  [Python for KasperskyOS](https://kas.pr/r46m).
>  For compatibility, we recommend setting the `CMAKE_FIND_ROOT_PATH` to
>  `~/.local/share/kos/$(basename $KOSCEDIR)/sysroot-aarch64-kos`.

After a successful build, the `kos-qemu-image` solution image will be located in the `./build/einit`
directory.

#### Hardware

To build the example to run on the target hardware platform, go to the directory with the example
and run the following commands:
```sh
cmake -B build \
      -D CMAKE_TOOLCHAIN_FILE=$KOSCEDIR/toolchain/share/toolchain-aarch64-kos.cmake \
      -D CMAKE_FIND_ROOT_PATH=~/.local/share/kos/$(basename $KOSCEDIR)/sysroot-aarch64-kos
cmake --build build --target {kos-image|sd-image} # choose one: kos-image or sd-image
```
Where:

* `kos-image` creates a KasperskyOS-based solution image that includes the example;
* `sd-image` creates a file system image for a bootable SD card.

> [!NOTE]
>  Ensure that `CMAKE_FIND_ROOT_PATH` points to where you installed the
>  [Python for KasperskyOS](https://kas.pr/r46m).
>  For compatibility, we recommend setting the `CMAKE_FIND_ROOT_PATH` to
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

[./einit/CMakeLists.txt](einit/CMakeLists.txt)—CMake commands for building the `Einit` program and
the solution image.

[./CMakeLists.txt](CMakeLists.txt)—CMake commands for building the solution.

## Usage

[Build and run](#building-and-running-the-example) the example. After running the example, the
message `Hello, world!` should be displayed in the standard output, demonstrating that Python is
running successfully on KasperskyOS.

[⬆ Back to Top](#table-of-contents)

© 2026 AO Kaspersky Lab
