# Using the KasperskyOS-adapted BoringSSL library

This example demonstrates the integration of the BoringSSL cryptographic library into a
KasperskyOS-based solution. During the build process, an adaptation patch from the
[KasperskyLab/boringssl-kos](https://kas.pr/o77g) repository is
automatically applied to the BoringSSL source code, ensuring its compatibility with KasperskyOS.

For additional details on KasperskyOS, including its limitations and known issues, please refer to
the [KasperskyOS Community Edition Online Help](https://kas.pr/si81).

The example uses the static variant of the BoringSSL library (the `initialize_platform` command with
the `FORCE_STATIC` parameter). For additional details regarding this command, please refer to the
[platform library](https://kas.pr/8r3i).

## Table of contents
- [Using the KasperskyOS-adapted BoringSSL library](#using-the-kasperskyos-adapted-boringssl-library)
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

`Hasher` is a program that calculates the SHA256 hash for the message `TestMessageHelloWorld`.

### Initialization description

The solution initialization description file named `init.yaml` is generated during the solution
build process based on the [`./einit/src/init.yaml.in`](einit/src/init.yaml.in) template.
The macros in `@INIT_*@`‌ ‌format contained in the template are automatically expanded in the
resulting `init.yaml` file. For more details, refer to
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

### Building and running the example

The example is built using the CMake build system, which is provided in the KasperskyOS Community
Edition SDK. When you develop a KasperskyOS-based solution, use the
[recommended structure of project directories](https://kas.pr/9zph)
to simplify the use of CMake scripts.

#### QEMU

To build the example to run on QEMU, go to the directory with the example and run the following
commands:
```sh
$ cmake -B build -D CMAKE_TOOLCHAIN_FILE="$KOSCEDIR/toolchain/share/toolchain-aarch64-kos.cmake"
$ cmake --build build --target {kos-qemu-image|sim}
```
where:

* `kos-qemu-image` creates a KasperskyOS-based solution image for QEMU that includes the example;
* `sim` creates a KasperskyOS-based solution image for QEMU that includes the example and runs it.

After a successful build, the `kos-qemu-image` solution image will be located at the `./build/einit`
directory.

#### Hardware

To build the example to run on the target hardware platform, go to the directory with the example
and run the following commands:
```sh
$ cmake -B build -D CMAKE_TOOLCHAIN_FILE="$KOSCEDIR/toolchain/share/toolchain-aarch64-kos.cmake"
$ cmake --build build --target {kos-image|sd-image}
```
where:

* `kos-image` creates a KasperskyOS-based solution image that includes the example;
* `sd-image` creates a file system image for a bootable SD card.

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

You can also use another alternative option to prepare and run the example:

1. Prepare the required hardware platform and a bootable SD card to run the example by following the
   instructions:
   - [Raspberry Pi 4 B](https://kas.pr/i8mg).
   - [Radxa ROCK 3A](https://kas.pr/21n7).
1. Run the example by following the instructions in the
[KasperskyOS Community Edition Online Help](https://kas.pr/9oa7).

[⬆ Back to Top](#Table-of-contents)

#### CMake input files

[./cmake/BoringSSL.cmake](cmake/BoringSSL.cmake)—CMake commands for including the BoringSSL library
in the project. This module implements the logic for the three supported integration methods. It
checks for a user-provided source directory via the `BORINGSSL_SOURCE_DIR` variable, attempts to
find a pre-installed package `BoringSSL`, and, as a fallback, automatically fetches the
KasperskyOS-adapted version of the library using the CMake module `FetchContent`.

[./hasher/CMakeLists.txt](hasher/CMakeLists.txt)—CMake commands for building the `Hasher` program.

[./einit/CMakeLists.txt](einit/CMakeLists.txt)—CMake commands for building the `Einit` program and
the solution image.

[./CMakeLists.txt](CMakeLists.txt)—CMake commands for building the solution.

## Usage

After running the example, the details about the message digest (or any errors) returned by the
`Hasher` will be displayed in the standard output:
```
Message parts: [Test][Message][Hello][World]
Message digest obtain with sha256 algorithm is: cb8f0c11401b96209ba9151f6eec442712502067287774ca2e61437b3feef2eb
```

[⬆ Back to Top](#table-of-contents)

© 2026 AO Kaspersky Lab
