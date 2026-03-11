# Using the KasperskyOS-adapted c-ares library

A simple asynchronous DNS resolver example demonstrating integration of the
[KasperskyOS-adapted c-ares library](https://github.com/KasperskyLab/c-ares-kos)
into a KasperskyOS-based solution.

The example also demonstrates the use of a callable handle, a client IPC handle that identifies both
an IPC channel to a server and an endpoint of that server. The server creates a callable handle and
passes it to the client so that the client can use the server endpoint. The client initializes the
IPC transport using the received callable handle. For more information, see
[Creating handles](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=libkos_handles_create).

## Table of contents
- [Using the KasperskyOS-adapted c-ares library](#using-the-kasperskyos-adapted-c-ares-library)
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

* `Client`—Program that takes a list of hostnames as command-line arguments, sends resolution
  requests for each to `NameResolver`, and prints the results (IP addresses or errors).
* `NameResolver`—Server program that accepts hostname resolution requests from `Client` and resolves
  them using the `cares_gethostbyname()` function.
* `Dhcpcd`—System program that implements a DHCP client, which gets network interface parameters
  from an external DHCP server in the background and passes them to a virtual file system.
* `VfsSdCardFs`—System program that supports the file system of SD cards.
* `VfsNet`—System program that supports network protocols.
* `SDCard`—SD Card driver.
* `BlobContainer`—System program required for working with dynamic libraries in shared memory.
* `EntropyEntity`—System program that implements random number generation.
* `DNetSrv`—Network card driver.

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
[init.yaml.in template](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=cmake_yaml_templates).

### Security policy description

The [`./einit/src/security.psl`](einit/src/security.psl) file describes the security policy of the
solution. The declarations in the PSL file are provided with comments that explain the purpose of
these declarations. For more information about the `security.psl` file, see
[Describing a security policy for a KasperskyOS-based solution](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=ssp_descr).

[⬆ Back to Top](#table-of-contents)

## Getting started

### Prerequisites

1. Confirm that your host system meets all the
[System requirements](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=system_requirements)
listed in the KasperskyOS Community Edition Developer's Guide.
1. [Install](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=sdk_install_and_remove)
the KasperskyOS Community Edition SDK version 1.4. You can download it for free from
[os.kaspersky.com](https://os.kaspersky.com/development/).
1. Copy the source files of this example to your local project directory.
1. Source the SDK setup script to configure the build environment. This exports the `KOSCEDIR`
  environment variable, which points to the SDK installation directory:
   ```sh
   source /opt/KasperskyOS-Community-Edition-<platform>-<version>/common/set_env.sh
   ```
1. [Build the necessary drivers](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=building_radxa_drivers)
from source only if you intend to run this example on Radxa ROCK 3A hardware. This step is not
required for QEMU or Raspberry Pi 4 B.
1. If not already installed, [build and install](https://github.com/KasperskyLab/c-ares-kos) the
KasperskyOS-adapted c-ares library, as it is a required dependency for this project.

### Building and running the example

The example is built using the CMake build system, which is provided in the KasperskyOS Community
Edition SDK. When you develop a KasperskyOS-based solution, use the
[recommended structure of project directories](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=cmake_using_sdk_cmake)
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
> [KasperskyOS-adapted c-ares library](https://github.com/KasperskyLab/c-ares-kos).
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
> [KasperskyOS-adapted c-ares library](https://github.com/KasperskyLab/c-ares-kos).
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
    * [Raspberry Pi 4 B](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=preparing_sd_card_rpi)
    * [Radxa ROCK 3A](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=preparing_sd_card_radxa)
1. Run the example by following the instructions in the
[KasperskyOS Community Edition Online Help](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=running_sample_programs_rpi)

#### CMake input files

[./client/CMakeLists.txt](client/CMakeLists.txt)—CMake commands for building the `Client` program.

[./einit/CMakeLists.txt](einit/CMakeLists.txt)—CMake commands for building the `Einit` program and
the solution image.

[./resolver/CMakeLists.txt](resolver/CMakeLists.txt)—CMake commands for building the `NameResolver`
program.

[./CMakeLists.txt](CMakeLists.txt)—CMake commands for building the solution.

## Usage

[Build and run](#building-and-running-the-example) the example, passing one or more hostnames as
command-line arguments. After running the example, resolution results for each hostname appear in
standard output, showing either IP addresses or error messages.

[⬆ Back to Top](#table-of-contents)

© 2026 AO Kaspersky Lab
