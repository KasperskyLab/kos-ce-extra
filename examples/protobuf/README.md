# Using the KasperskyOS-adapted Protocol Buffers

This solution demonstrates how to integrate the
[KasperskyOS-adapted version of Protocol Buffers (Protobuf™)](https://kas.pr/fgv5)
into a KasperskyOS-based solution.

The KasperskyOS-adapted protobuf libraries can be built as either a static or a dynamic libraries.
This solution is configured for dynamic linking by default. For more information, see
[Using dynamic libraries](https://kas.pr/91x7).

To build a fully static solution with static variant of the protobuf libraries, use the
`initialize_platform()` with the `FORCE_STATIC` parameter in [./CMakeLists.txt](CMakeLists.txt) and
make sure you make the necessary changes to the security policy description file
[`./einit/src/security.psl`](einit/src/security.psl).

For additional details regarding `initialize_platform()` command, please refer to the
[platform library](https://kas.pr/8r3i).

## Table of contents
- [Using the KasperskyOS-adapted Protocol Buffers](#using-the-kasperskyos-adapted-protocol-buffers)
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

* `Producer`—Program that reads the address book file on the SD card and converts it into a protobuf
  message, which it then sends to the `Consumer` program
* `Consumer`—Program that prints the message received from the `Producer` program.
* `VfsSdCardFs`—System program that supports the file system of SD cards.
* `Server`—System program that implements a
  [log server](https://kas.pr/g3vg)
  to which other programs forward messages. To send messages to the log server, programs use the
  [logrr_cpp](https://kas.pr/8da2)
  library, which filters messages by
  [log level](https://kas.pr/8da2).
  The log server forwards the received messages to the output channel named `FsOutputChannel`.
* `FsOutputChannel`—System program that implements an output channel for the Server program. The
  output channel saves messages received from the log server to a file.
* `DCM`—System program for dynamic creation of IPC channels.
* `BlobContainer`—System program required for working with dynamic libraries in shared memory.
* `SDCard`—SD card driver.
* `EntropyEntity`—System program that implements random number generation.

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
1. If not already installed, [build and install](https://kas.pr/fgv5) the
KasperskyOS-adapted protobuf, as it is a required dependency for this project.

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
>  [Protobuf compiler](https://kas.pr/fgv5#protobuf-compiler).
>  For compatibility with the KasperskyOS adaptation patch, we recommend setting it to
>  `~/.local/share/kos/$(basename $KOSCEDIR)/toolchain`.
> * `CMAKE_FIND_ROOT_PATH` must point to the installation directory of the
>  [Protobuf C++ runtime](https://kas.pr/fgv5#protobuf-c-runtime).
>  For compatibility, we recommend setting it to
>  `~/.local/share/kos/$(basename $KOSCEDIR)/sysroot-aarch64-kos`.

After a successful build, the `kos-qemu-image` solution image will be located in the `./build/einit`
directory.

#### Hardware

To build the example to run on the target hardware platform, go to the directory with the example and
run the following commands:
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
>  [Protobuf compiler](https://kas.pr/fgv5#protobuf-compiler).
>  For compatibility with the KasperskyOS adaptation patch, we recommend setting it to
>  `~/.local/share/kos/$(basename $KOSCEDIR)/toolchain`.
> * `CMAKE_FIND_ROOT_PATH` must point to the installation directory of the
>  [Protobuf C++ runtime](https://kas.pr/fgv5#protobuf-c-runtime).
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

[⬆ Back to Top](#table-of-contents)

#### CMake input files

[./consumer/CMakeLists.txt](consumer/CMakeLists.txt)—CMake commands for building the `Consumer`
program.

[./producer/CMakeLists.txt](producer/CMakeLists.txt)—CMake commands for building the `Producer`
program.

[./einit/CMakeLists.txt](einit/CMakeLists.txt)—CMake commands for building the `Einit` program and
the solution image.

[./CMakeLists.txt](CMakeLists.txt)—CMake commands for building the solution.

## Usage

[Build and run](#building-and-running-the-example) the example. All logged messages are written to a
log file. The file's location is determined by the `LOG_DIR` parameter of the
`create_logrr_fs_output_channel()` function in the [`./einit/CMakeLists.txt`](einit/CMakeLists.txt)
file.

[⬆ Back to Top](#table-of-contents)

© 2026 AO Kaspersky Lab
