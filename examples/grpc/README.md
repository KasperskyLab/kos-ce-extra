# gRPC Hello World example

This solution demonstrates how to integrate the
[KasperskyOS-adapted version of gRPC® (Google Remote Procedure Call) ](https://github.com/KasperskyLab/grpc-kos)
into a KasperskyOS-based solution.

To illustrate this integration, the solution provides a practical example — a modified version of
the gRPC HelloWorld C++ example, adapted for KasperskyOS. It demonstrates four distinct interaction
scenarios between a client and a server, using either a secure channel with Secure Sockets Layer
(SSL)/Transport Layer Security (TLS) authentication or an insecure channel:

* Scenario 1: Insecure communication between a Linux server and a KasperskyOS client.
* Scenario 2: Insecure communication between a KasperskyOS server and a Linux client.
* Scenario 3: Secure communication between a Linux server and a KasperskyOS client.
* Scenario 4: Secure communication between a KasperskyOS server and a Linux client.

## Table of contents
- [gRPC Hello World example](#grpc-hello-world-example)
  - [Table of contents](#table-of-contents)
  - [Solution overview](#solution-overview)
    - [List of programs](#list-of-programs)
    - [General scenario](#general-scenario)
    - [Initialization description](#initialization-description)
    - [Security policy description](#security-policy-description)
  - [Getting started](#getting-started)
    - [Prerequisites](#prerequisites)
    - [Building and running the example](#building-and-running-the-example)
      - [Linux host](#linux-host)
      - [KasperskyOS](#kasperskyos)
        - [QEMU](#qemu)
        - [Hardware](#hardware)
      - [CMake input files](#cmake-input-files)
  - [Usage](#usage)
    - [Scenario 1](#scenario-1)
    - [Scenario 2](#scenario-2)
    - [Scenario 3](#scenario-3)
    - [Scenario 4](#scenario-4)

## Solution overview

### List of programs

* `GreeterClient`—Program that serves as the client in different scenarios, acting as the
KasperskyOS client in scenarios 1 and 3, and as the Linux client in scenarios 2 and 4.
* `GreeterServer`—Program that serves as the server that implements a gRPC service in different
scenarios, acting as the KasperskyOS server in scenarios 2 and 4, and as the Linux server in
scenarios 1 and 3.
* `VfsNet`—System program that supports network protocols.
* `Dhcpcd`—System program that implements a DHCP client, which gets network interface parameters
  from an external DHCP server in the background and passes them to a virtual file system.
* `DNetSrv`—Driver for working with network cards.
* `VfsSdCardFs`—System program that supports the file system of SD cards.
* `BlobContainer`—System program required for working with dynamic libraries in shared memory.
* `SDCard`—SD card driver.
* `EntropyEntity`—System program that implements random number generation.
* `Ntpd`—System program that implements an NTP client that receives time parameters from external
  NTP servers in the background and forwards them to the KasperskyOS kernel.

When you build the example for the target hardware platform, platform-specific drivers are
automatically included in the solution:

* `BSP`—Hardware platform support package (Board Support Package). Provides cross-platform
configuration of peripherals for the Radxa ROCK 3A and Raspberry Pi 4 B.
* `GPIO`—GPIO support driver for the Radxa ROCK 3A.
* `PinCtrl`—Low-level pin multiplexing (pinmux) configuration driver for the Radxa ROCK 3A.
* `Bcm2711MboxArmToVc`—Driver for working with the VideoCore (VC6) coprocessor via mailbox
technology for Raspberry Pi 4 B.

[⬆ Back to Top](#table-of-contents)

### General scenario

This is a general scenario that showcases how the client and server interact with each other by
exchanging requests and responses using the gRPC protocol:

1. The server is launched and starts listening on the specified address for incoming connections.
   The server can be run in secure mode by providing the `--secure` command line argument. In secure
   mode, it uses SSL/TLS for secure communication with the client.
1. The client is launched and creates a channel to connect to the server specified by the `--target`
  command line argument. The client can be run in secure mode by providing the `--secure` command
  line argument. In secure mode, it uses SSL/TLS for secure communication with the server.
1. The client sends requests to the server using the `SayHello()` method and waits for the response.
1. The server receives the request from the client, processes it by adding a prefix to the name in
  the request, and returns a response with the prefix.
1. The client receives the response from the server and displays it in the standard output.
   The client repeats this process 20,000 times with a 2-second interval between each request.
1. Once the interaction is completed, both the client and the server close the connection and
  terminate their execution.

### Initialization description

The solution initialization description file named `init.yaml` is generated during the solution
build process based on the [`./einit/src/client.init.yaml.in`](einit/src/client.init.yaml.in) and
the [`./einit/src/server.init.yaml.in`](einit/src/server.init.yaml.in) templates. Macros in the
`@INIT_*@`‌ ‌format contained in the template are automatically expanded in the resulting
`init.yaml` file. For more details, refer to
[init.yaml.in template](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=cmake_yaml_templates).

### Security policy description

The `security.psl` file contains the [solution security policy description](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=ssp_descr)
and is generated based on the [`./einit/src/security.psl.in`](einit/src/security.psl.in) template
during the solution build process. For more details, refer to [security.psl.in template](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=cmake_psl_templates).

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
1. If not already installed, [build and install](https://github.com/KasperskyLab/grpc-kos) the
KasperskyOS-adapted gRPC, as it is a required dependency for this project.

[⬆ Back to Top](#table-of-contents)

### Building and running the example

The example is built using the CMake build system, which is provided in the
KasperskyOS Community Edition SDK. When you develop a KasperskyOS-based solution, use the
[recommended structure of project directories](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=cmake_using_sdk_cmake)
to simplify the use of CMake scripts.

#### Linux host

To build the client or server for the Linux host, execute the following commands:

```sh
$ cmake -B build/host \
        -D CMAKE_PREFIX_PATH="~/.local/share/kos/$(basename $KOSCEDIR)/toolchain"
$ cmake --build build/host
```

> [!NOTE]
> `CMAKE_PREFIX_PATH` must point to the installation directory of
> [gRPC for Linux](https://github.com/KasperskyLab/grpc-kos/README.md#build-grpc-for-linux-host-operating-system).
> For compatibility with the KasperskyOS adaptation patch, we recommend setting it to
> `~/.local/share/kos/$(basename $KOSCEDIR)/toolchain`.

As a result, two executables will be built:

* `build/host/client/GreeterClient`—the Linux client;
* `build/host/server/GreeterServer`—the Linux server.

To run an executable, use:
```sh
# For the Linux server
$ cd build/host/server/
$ ./GreeterServer [--secure]

# For the Linux client
$ cd build/host/client/
$ ./GreeterClient [--secure]
```

The `--secure` command line flag enables SSL/TLS encryption for the gRPC channel. Omit this flag for
an insecure (plaintext) connection.

#### KasperskyOS

##### QEMU

To build a KasperskyOS-based solution with a gRPC server or client, execute the following commands:
```sh
$ cmake -B build/cross \
        -D BUILD_TARGET="{client|server}" \
        -D USE_SECURE_CONNECTION={YES|NO} \
        -D CMAKE_PREFIX_PATH="~/.local/share/kos/$(basename $KOSCEDIR)/toolchain" \
        -D CMAKE_FIND_ROOT_PATH="~/.local/share/kos/$(basename $KOSCEDIR)/sysroot-aarch64-kos" \
        -D CMAKE_TOOLCHAIN_FILE="${KOSCEDIR}/toolchain/share/toolchain-aarch64-kos.cmake"
$ cmake --build build/cross --target {kos-qemu-image|sim}
```

where:

* `BUILD_TARGET` defines the component to build:
    * `client` creates the KasperskyOS client;
    * `server` creates the KasperskyOS server.
* `USE_SECURE_CONNECTION` controls SSL/TLS:
    * `YES` enables SSL/TLS for an encrypted gRPC channel;
    * `NO` configures an insecure gRPC channel.
* `--target` argument specifies the output:
    * `kos-qemu-image` creates a KasperskyOS solution image for QEMU containing the example;
    * `sim` creates a KasperskyOS solution image for QEMU containing the example and runs it.

> [!NOTE]
> * `CMAKE_PREFIX_PATH` must point to the installation directory of
>  [gRPC for Linux](https://github.com/KasperskyLab/grpc-kos/README.md#build-grpc-for-linux-host-operating-system).
>  For compatibility with the KasperskyOS adaptation patch, we recommend setting it to
>  `~/.local/share/kos/$(basename $KOSCEDIR)/toolchain`.
> * `CMAKE_FIND_ROOT_PATH` must point to the installation directory of
>  [gRPC for KasperskyOS](https://github.com/KasperskyLab/grpc-kos/README.md#build-grpc-for-kasperskyos).
>  For compatibility, we recommend setting it to
>  `~/.local/share/kos/$(basename $KOSCEDIR)/sysroot-aarch64-kos`.

After a successful build, the `kos-qemu-image` solution image will be located in the
`./build/cross/einit` directory.

##### Hardware

To build the example to run on the target hardware platform, go to the directory with the example
and run the following commands:
```sh
$ cmake -B build/cross \
        -D BUILD_TARGET="{client|server}" \
        -D USE_SECURE_CONNECTION={YES|NO} \
        -D CMAKE_PREFIX_PATH="~/.local/share/kos/$(basename $KOSCEDIR)/toolchain" \
        -D CMAKE_FIND_ROOT_PATH="~/.local/share/kos/$(basename $KOSCEDIR)/sysroot-aarch64-kos" \
        -D CMAKE_TOOLCHAIN_FILE="${KOSCEDIR}/toolchain/share/toolchain-aarch64-kos.cmake"
$ cmake --build build/cross --target {kos-image|sd-image}
```

where:

* `BUILD_TARGET` defines the component to build:
    * `client` creates the KasperskyOS client;
    * `server` creates the KasperskyOS server.
* `USE_SECURE_CONNECTION` controls SSL/TLS:
    * `YES` enables SSL/TLS for an encrypted gRPC channel;
    * `NO` configures an insecure gRPC channel.
* `--target` argument specifies the output:
    * `kos-image` creates a KasperskyOS-based solution image that includes the example;
    * `sd-image` creates a file system image for a bootable SD card.

> [!NOTE]
> * `CMAKE_PREFIX_PATH` must point to the installation directory of
>  [gRPC for Linux](https://github.com/KasperskyLab/grpc-kos/README.md#build-grpc-for-linux-host-operating-system).
>  For compatibility with the KasperskyOS adaptation patch, we recommend setting it to
>  `~/.local/share/kos/$(basename $KOSCEDIR)/toolchain`.
> * `CMAKE_FIND_ROOT_PATH` must point to the installation directory of
>  [gRPC for KasperskyOS](https://github.com/KasperskyLab/grpc-kos/README.md#build-grpc-for-kasperskyos).
>  For compatibility, we recommend setting it to
>  `~/.local/share/kos/$(basename $KOSCEDIR)/sysroot-aarch64-kos`.
> * For scenarios 1 and 3 (KasperskyOS client), ensure the `targetEndpoint` variable in
>  [`./client/src/main.cc`](client/src/main.cc) points to your Linux server's IP address (default
>  is `10.0.2.2` for QEMU). To change it, configure CMake with
>  `-D DEFAULT_ENDPOINT_ADDRESS="<IP:PORT>"`.

After a successful build, the `kos-image` solution image will be located in the
`./build/cross/einit` directory. The `hdd.img` bootable SD card image will be located in the
`./build/cross` directory.

To run the example on hardware after building the `kos-image` target:

1. Prepare the required hardware platform and bootable SD card by following the instructions in the
  KasperskyOS Community Edition Online Help:
    * [Raspberry Pi 4 B](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=preparing_sd_card_rpi)
    * [Radxa ROCK 3A](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=preparing_sd_card_radxa)
1. Copy the `kos-image` to the bootable SD card.
1. Connect the bootable SD card to the hardware.
1. Supply power to the hardware and wait for the example to run.

To run the example on hardware after building the `sd-image` target:

1. Connect the SD card to the computer.
1. Copy the `hdd.img` bootable SD card image to the SD card using the command:
   ```sh
   $ sudo dd bs=64k if=build/cross/hdd.img of=/dev/sd[X] conv=fsync
   ```
   where `[X]` is the final character in the name of the SD card block device.
1. Connect the bootable SD card to the hardware.
1. Supply power to the hardware and wait for the example to run.

#### CMake input files

* [./client/CMakeLists.txt](client/CMakeLists.txt)

  CMake commands for building the `GreeterClient` program.
* [./einit/CMakeLists.txt](einit/CMakeLists.txt)

  CMake commands for building the `Einit` program and the solution image.
* [./libraries/CMakeLists.txt](libraries/CMakeLists.txt)

  [./libraries/hello_world_proto/CMakeLists.txt](libraries/hello_world_proto/CMakeLists.txt)

  [./libraries/utility_lib/CMakeLists.txt](libraries/utility_lib/CMakeLists.txt)

  CMake commands for building the shared libraries used by the `GreeterClient` and the
  `GreeterServer` programs.
* [./server/CMakeLists.txt](server/CMakeLists.txt)

  CMake commands for building the `GreeterServer` program.
* [./CMakeLists.txt](CMakeLists.txt)

  CMake commands for building the solution.

[⬆ Back to Top](#table-of-contents)

## Usage

### Scenario 1

1. [Build and run](#linux-host) the Linux server without the `--secure` flag.
1. Wait for the standard output to display that the server has started listening:
    ```
    ...
    [GreeterServer] starting...
    [GreeterServer] using secure connection: false
    [GreeterServer] listening on 0.0.0.0:50051
    ```
1. Build the KasperskyOS client for an insecure connection (`USE_SECURE_CONNECTION=NO`) and run it
  either in [QEMU](#qemu) or on [hardware](#hardware).
1. The standard output should display a count of messages sent and their corresponding responses:
    ```
    ...
    [GreeterClient] starting...
    [GreeterClient] waiting for network...
    [GreeterClient] target: 10.0.2.2:50051
    [GreeterClient] using secure connection: false
    [GreeterClient] send request 1
    [GreeterClient] received: Hello KasperskyOS world
    [GreeterClient] send request 2
    [GreeterClient] received: Hello KasperskyOS world
    ...
    ```

### Scenario 2

1. Build the KasperskyOS server for an insecure connection (`USE_SECURE_CONNECTION=NO`) and run it
  either in [QEMU](#qemu) or on [hardware](#hardware).
1. Wait for the standard output to display that the server has started listening:
    ```
    ...
    [GreeterServer] starting...
    [GreeterServer] using secure connection: false
    [GreeterServer] listening on 0.0.0.0:50051
    ```
1. [Build and run](#linux-host) the Linux client from a separate terminal without the `--secure`
  flag.
1. The standard output should display a count of messages sent and their corresponding responses:
    ```
    ...
    [GreeterClient] starting...
    [GreeterClient] target: localhost:50051
    [GreeterClient] using secure connection: false
    [GreeterClient] send request 1
    [GreeterClient] received: KOS-Hello world
    [GreeterClient] send request 2
    [GreeterClient] received: KOS-Hello world
    ...
    ```

 ### Scenario 3

1. [Build and run](#linux-host) the Linux server with the `--secure` flag.
1. Wait for the standard output to display that the server has started listening:
    ```
    ...
    [GreeterServer] starting...
    [GreeterServer] using secure connection: true
    [GreeterServer] listening on 0.0.0.0:50051
    ```
1. Build the KasperskyOS client for a secure connection (`USE_SECURE_CONNECTION=YES`) and run it
  either in [QEMU](#qemu) or on [hardware](#hardware).
1. The standard output should display a count of messages sent and their corresponding responses:
    ```
    ...
    [GreeterClient] starting...
    [GreeterClient] waiting for network...
    [GreeterClient] target: 10.0.2.2:50051
    [GreeterClient] using secure connection: true
    [GreeterClient] send request 1
    [GreeterClient] received: Hello KasperskyOS world
    [GreeterClient] send request 2
    [GreeterClient] received: Hello KasperskyOS world
    ...
    ```

### Scenario 4

1. Build the KasperskyOS server for a secure connection (`USE_SECURE_CONNECTION=YES`) and run it
  either in [QEMU](#qemu) or on [hardware](#hardware).
1. Wait for the standard output to display that the server has started listening:
    ```
    ...
    [GreeterServer] starting...
    [GreeterServer] using secure connection: true
    [GreeterServer] listening on 0.0.0.0:50051
    ```
1. [Build and run](#linux-host) the Linux client from a separate terminal with
  the `--secure` flag.
1. The standard output should display a count of messages sent and their corresponding responses:
    ```
    ...
    [GreeterClient] starting...
    [GreeterClient] target: localhost:50051
    [GreeterClient] using secure connection: true
    [GreeterClient] send request 1
    [GreeterClient] received: KOS-Hello world
    [GreeterClient] send request 2
    [GreeterClient] received: KOS-Hello world
    ...
    ```

[⬆ Back to Top](#table-of-contents)

© 2026 AO Kaspersky Lab
