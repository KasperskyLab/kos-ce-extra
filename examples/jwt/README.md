# Using the KasperskyOS-adapted CPP-JWT library

This project demonstrates how to use the [CPP-JWT](https://github.com/arun11299/cpp-jwt) library in
a KasperskyOS-based solution. The project is based on the
[commit](https://github.com/arun11299/cpp-jwt/commit/4b66cf74e5ece16e7f7e8c3d8c0c63d01b4cc9aa)
of version v1.5.1.

For additional details on KasperskyOS, including its limitations and known issues, please refer to
the [KasperskyOS Community Edition Online Help](https://kas.pr/si81).

## Table of contents
- [Using the KasperskyOS-adapted CPP-JWT library](#using-the-kasperskyos-adapted-cpp-jwt-library)
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

* `WebServer`—Web server.
* `TokenService`—User authentication subsystem.
* `VfsSDCardFs`—System program that supports the file system of SD cards.
* `VfsNet`—System program that supports network protocols.
* `Dhcpcd`—System program that implements a DHCP client, which gets network interface parameters
from an external DHCP server in the background and passes them to a virtual file system.
* `BlobContainer`—System program required for working with dynamic libraries in shared memory.
* `SDCard`—SD card driver.
* `EntropyEntity`—System program that implements random number generation.
* `DNetSrv`—Network card driver.
* `DCM`—System program that lets you dynamically create IPC channels.
* `Server`—System program that implements a log server to which the Client program sends messages.
The log server forwards the messages to the output channel named `FsOutputChannel`.
* `FsOutputChannel`—System program that implements an output channel for the Server program. The
output channel saves messages received from the log server to a file.

When you build the example for the target hardware platform, platform-specific drivers are
automatically included in the solution:

* `BSP`—Hardware platform support package (Board Support Package). Provides cross-platform
configuration of peripherals for the Radxa ROCK 3A and Raspberry Pi 4 B.
* `GPIO`—GPIO support driver for the Radxa ROCK 3A.
* `PinCtrl`—Low-level pin multiplexing (pinmux) configuration driver for the Radxa ROCK 3A.
* `Bcm2711MboxArmToVc`—Driver for working with the VideoCore (VC6) coprocessor via mailbox
technology for Raspberry Pi 4 B.

[⬆ Back to Top](#Table-of-contents)

### Initialization description

The solution initialization description file named `init.yaml` is generated during the solution
build process based on the [`./einit/src/init.yaml.in`](einit/src/init.yaml.in) template.
The macros in `@INIT_*@`‌ ‌format contained in the template are
automatically expanded in the resulting `init.yaml` file. For more details, refer to
[init.yaml.in template](https://kas.pr/nb5m).

### Security policy description

The [`./einit/src/security.psl`](einit/src/security.psl) file describes the security policy of the
solution. The declarations in the PSL file are provided with comments that explain the purpose of
these declarations. For more information about the `security.psl` file, see
[Describing a security policy for a KasperskyOS-based solution](https://kas.pr/jm1v).

[⬆ Back to Top](#Table-of-contents)

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
$ cmake -B build \
        -D CMAKE_TOOLCHAIN_FILE="$KOSCEDIR/toolchain/share/toolchain-aarch64-kos.cmake"
$ cmake --build build --target {kos-qemu-image|sim}
```
where:

* `kos-qemu-image` creates a KasperskyOS-based solution image for QEMU that includes the example;
* `sim` creates a KasperskyOS-based solution image for QEMU that includes the example and runs it.

After a successful build, the `kos-qemu-image` solution image will be located in the `./build/einit`
directory.

#### Hardware

To build the example to run on the target hardware platform, go to the directory with the example
and run the following commands:
```sh
$ cmake -B build \
        -D CMAKE_TOOLCHAIN_FILE="$KOSCEDIR/toolchain/share/toolchain-aarch64-kos.cmake"
$ cmake --build build --target {kos-image|sd-image}
```
where:

* `kos-image` creates a KasperskyOS-based solution image that includes the example;
* `sd-image` creates a file system image for a bootable SD card.

After a successful build, the `kos-image` solution image will be located in the `./build/einit`
directory. The `hdd.img` bootable SD card image will be located in the `./build` directory.

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

1. Prepare the required hardware platform and a bootable SD card to run the example by following the
instructions:

   - [Raspberry Pi 4 B](https://kas.pr/i8mg).
   - [Radxa ROCK 3A](https://kas.pr/21n7).
1. Run the example by following the instructions in the
[KasperskyOS Community Edition Online Help](https://kas.pr/9oa7).

[⬆ Back to Top](#Table-of-contents)

#### CMake input files

[./cmake/cpp-jwt.cmake](cmake/cpp-jwt.cmake)—CMake commands for including the CPP-JWT
library in the project. This module implements the logic for the two supported integration methods.
It checks for a user-provided source directory via the `JWT_SOURCE_DIR` variable, and automatically
fetches the specific version of the CPP-JWT library using the CMake module `FetchContent`.

[./einit/CMakeLists.txt](einit/CMakeLists.txt)—CMake commands for building the `Einit` program
and the solution image.

[./token_service/CMakeLists.txt](token_service/CMakeLists.txt)—CMake commands for building the
`TokenService` program.

[./web_server/CMakeLists.txt](web_server/CMakeLists.txt)—CMake commands for building the
`WebServer` program.

[./CMakeLists.txt](CMakeLists.txt)—CMake commands for building the solution.

## Usage

After building and running the example, follow these steps:

1. Wait until the following message appears in the standard output:
   ```
   [WebServer] WebServer started (port: 1106)
   ```
1. Open the page <https://localhost:1106> (when running the example on QEMU) or
[https://\<hardware_IP_address\>:1106]() (when running the example on Raspberry Pi 4 B or Radxa ROCK
3A) in your browser. The browser will display the `index` page containing an authentication prompt.
You may need to accept an insecure connection upon first time opening the page, or, alternatively,
import the root certificate located in `resources/hdd/certs` into your browser trusted certificate
chain. If you don't import the certificate, you may see the following warnings in the logs:
   ```
   [WebServer] handshake: sslv3 alert certificate unknown (SSL routines, ssl3_read_bytes)
   ```
1. Click the `Get data` button. The browser will display the `Wrong token or token service
unavailable` message with the following information in the standard output:

   ```
   [WebServer]  Local URL: /getdata
   [WebServer]  Request: GET
   [TokenService] Verify
   [TokenService] Token verify error: signature format is incorrect
   ```
1. Click the `Get JWT` button. The `TokenService` program uses the values in the variables
`payloadKey` and `payloadValue` to create the JWT token. The browser will display the generated
token in the `JWT` field. The standard output will show information about the generated JWT token:

   ```
   [WebServer]  Local URL: /gettoken
   [WebServer]  Request: GET
   [TokenService] Generate
   ```
1. Click the `Get data` button. The browser will display the `Data access succeeded` with the
following information in the standard output:

   ```
   [WebServer]  Local URL: /getdata
   [WebServer]  Request: GET
   [TokenService] Verify
   [TokenService]  HEADER: {"alg":"HS256","typ":"JWT"}
   [TokenService] PAYLOAD: {"secret":"TOKEN"}
   ```
[⬆ Back to Top](#Table-of-contents)

© 2026 AO Kaspersky Lab
