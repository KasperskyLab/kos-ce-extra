# Using the KasperskyOS-adapted Abseil C++ library

This project demonstrates how to integrate the
[KasperskyOS-adapted version of the Abseil C++ Common Libraries](https://kas.pr/o98f)
into a KasperskyOS-based solution. Based on the official
[Abseil C++ Quickstart With CMake](https://abseil.io/docs/cpp/quickstart-cmake.html#c-quickstart-with-cmake),
this example prints `Joined string: foo-bar-baz` to standard output when executed.

This project uses dynamic linking by default, supporting both dynamic and static variants of the
KasperskyOS-adapted Abseil C++ library. To build a fully static solution, specify the `FORCE_STATIC`
parameter for the `initialize_platform()` command in the [./CMakeLists.txt](CMakeLists.txt) file.
For more information, see the
[platform library](https://kas.pr/8r3i)
documentation.

For additional details on KasperskyOS, including its limitations and known issues, please refer to
the [KasperskyOS Community Edition Online Help](https://kas.pr/si81).

## Table of contents
- [Using the KasperskyOS-adapted Abseil C++ library](#using-the-kasperskyos-adapted-abseil-c-library)
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

* `Hello`—Program that joins the strings `foo`, `bar`, `baz` and prints the resulting string to the
standard output.
* `DCM`—System program that lets you dynamically create IPC channels.
* `BlobContainer`—System program required for working with dynamic libraries in shared memory.
* `EntropyEntity`—System program that implements random number generation.
* `VfsSDCardFs`—System program that supports the file system of SD cards.
* `SDCard`—SD Card driver.

When you build the example for the target hardware platform, platform-specific drivers are
automatically included in the solution:

* `BSP`—Hardware platform support package (Board Support Package). Provides cross-platform
configuration of peripherals for the Radxa ROCK 3A and Raspberry Pi 4 B.
* `GPIO`—GPIO support driver for the Radxa ROCK 3A.
* `PinCtrl`—Low-level pin multiplexing (pinmux) configuration driver for the Radxa ROCK 3A.
* `Bcm2711MboxArmToVc`—Driver for working with the VideoCore (VC6) coprocessor via mailbox
technology for Raspberry Pi 4 B.

### Initialization description

The solution initialization description file named `init.yaml` is generated during the solution
build process based on the [`./einit/src/init.yaml.in`](einit/src/init.yaml.in) template. Macros in
the `@INIT_*@`‌ ‌format contained in the template are automatically expanded in the resulting
`init.yaml` file. For more details, refer to [init.yaml.in
template](https://kas.pr/nb5m).

### Security policy description

The [`./einit/src/security.psl`](einit/src/security.psl) file contains a
[description of a security policy for a KasperskyOS-based solution](https://kas.pr/jm1v).

The `security.psl` file is a top-level file that includes a part of the solution security policy
description in the form of a PSL file [`./einit/src/dynld.psl`](einit/src/dynld.psl) via the
[use declaration](https://kas.pr/g6a3).
The `dynld.psl` file contains the security policy description that is used when dynamically linking
the solution components.

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
1. If not already installed, [build and install](https://kas.pr/o98f) the
KasperskyOS-adapted Abseil C++ library, as it is a required dependency for this project.

### Building and running the example

The example is built using the CMake build system, which is provided in the KasperskyOS Community
Edition SDK. When you develop a KasperskyOS-based solution, use the
[recommended structure of project directories](https://kas.pr/9zph)
to simplify the use of CMake scripts.

#### QEMU

To build the example to run on QEMU, go to the directory with the example and run the following
commands:
``` sh
$ cmake -B build \
        -D CMAKE_FIND_ROOT_PATH="~/.local/share/kos/$(basename $KOSCEDIR)/sysroot-aarch64-kos" \
        -D CMAKE_TOOLCHAIN_FILE="${KOSCEDIR}/toolchain/share/toolchain-aarch64-kos.cmake"
$ cmake --build build --target {kos-qemu-image|sim}
```
where:

* `kos-qemu-image` creates a KasperskyOS-based solution image for QEMU that includes the example;
* `sim` creates a KasperskyOS-based solution image for QEMU that includes the example and runs it.

> [!NOTE]
> Ensure that `CMAKE_FIND_ROOT_PATH` points to where you installed the
> [KasperskyOS-adapted Abseil C++ library](https://kas.pr/o98f).
> For compatibility, we recommend setting the `CMAKE_FIND_ROOT_PATH` to
> `~/.local/share/kos/$(basename $KOSCEDIR)/sysroot-aarch64-kos`.

After a successful build, the `kos-qemu-image` solution image will be located at the `./build/einit`
directory.

[⬆ Back to Top](#table-of-contents)

#### Hardware

To build the example to run on the target hardware platform, go to the directory with the example
and run the following commands:
``` sh
$ cmake -B build \
        -D CMAKE_FIND_ROOT_PATH="~/.local/share/kos/$(basename $KOSCEDIR)/sysroot-aarch64-kos" \
        -D CMAKE_TOOLCHAIN_FILE="${KOSCEDIR}/toolchain/share/toolchain-aarch64-kos.cmake"
$ cmake --build build --target {kos-image|sd-image}
```
where:

* `kos-image` creates a KasperskyOS-based solution image that includes the example;
* `sd-image` creates a file system image for a bootable SD card.

> [!NOTE]
> Ensure that `CMAKE_FIND_ROOT_PATH` points to where you installed the
> [KasperskyOS-adapted Abseil C++ library](https://kas.pr/o98f).
> For compatibility, we recommend setting the `CMAKE_FIND_ROOT_PATH` to
> `~/.local/share/kos/$(basename $KOSCEDIR)/sysroot-aarch64-kos`.

After a successful build, the `kos-image` solution image will be located at the `./build/einit`
directory. The `hdd.img` bootable SD card image will be located at the `./build` directory.

To run the example on the target hardware platform:

1. Connect the SD card to the computer.
1. Copy the bootable SD card image to the SD card using the command:
   ```sh
   $ sudo dd bs=64k if=build/hdd.img of=/dev/sd[X] conv=fsync
   ```
   where `[X]` is the final character in the name of the SD card block device.

1. Connect the bootable SD card to the board.
1. Supply power to the board and wait for the example to run.

You can also use an alternative option to prepare and run the example:

1. Prepare the required hardware platform and bootable SD card by following the instructions in the
KasperskyOS Community Edition Online Help:
    * [Raspberry Pi 4 B](https://kas.pr/i8mg)
    * [Radxa ROCK 3A](https://kas.pr/21n7)
1. Run the example by following the instructions in the
[KasperskyOS Community Edition Online Help](https://kas.pr/9oa7)

#### CMake input files

[./hello/CMakeLists.txt](hello/CMakeLists.txt)—CMake commands for building the `Hello` program.

[./einit/CMakeLists.txt](einit/CMakeLists.txt)—CMake commands for building the `Einit` program and
the solution image.

[./CMakeLists.txt](CMakeLists.txt)—CMake commands for building the solution.

## Usage

[Build and run](#building-and-running-the-example) the example. After running the example, the
message `Joined string: foo-bar-baz` should be displayed in the standard output.

[⬆ Back to Top](#table-of-contents)

© 2026 AO Kaspersky Lab
