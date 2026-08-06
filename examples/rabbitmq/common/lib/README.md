# KasperskyOS adaptation of RabbitMQ-C AMQP client library

This is a KasperskyOS-adapted version of the [RabbitMQ® C AMQP client library](https://github.com/alanxz/rabbitmq-c),
based on tag [v0.15.0](https://github.com/alanxz/rabbitmq-c/releases/tag/v0.15.0).

The KasperskyOS-adapted RabbitMQ-C library enables programs to send and receive messages via a
RabbitMQ message broker. For more information, see the [RabbitMQ](https://www.rabbitmq.com/).

Communication between the KasperskyOS-adapted RabbitMQ-C library and the RabbitMQ message broker is
based on the Advanced Message Queuing Protocol (AMQP). This implementation supports AMQP version
0.9.1. For protocol details, see [AMQP Concepts](https://www.rabbitmq.com/tutorials/amqp-concepts).

For additional details on KasperskyOS, including its limitations and known issues, please refer to
the [KasperskyOS Community Edition Online Help](https://kas.pr/si81).

## Table of contents
- [KasperskyOS adaptation of RabbitMQ-C AMQP client library](#kasperskyos-adaptation-of-rabbitmq-c-amqp-client-library)
  - [Table of contents](#table-of-contents)
  - [Getting started](#getting-started)
    - [Prerequisites](#prerequisites)
    - [Building and installing](#building-and-installing)
      - [CMake input files](#cmake-input-files)
  - [Usage](#usage)

## Getting started

### Prerequisites

1. Confirm that your host system meets all the
[System requirements](https://kas.pr/u87d)
listed in the KasperskyOS Community Edition Developer's Guide.
1. [Install](https://kas.pr/ce73)
the KasperskyOS Community Edition SDK version 1.4. You can download it for free from
[os.kaspersky.com](https://kas.pr/4mro).
1. Source the SDK setup script to configure the build environment. This exports the `KOSCEDIR`
  environment variable, which points to the SDK installation directory:
   ```sh
   source /opt/KasperskyOS-Community-Edition-<platform>-<version>/common/set_env.sh
   ```
1. Clone the RabbitMQ-C repository to your project directory (to reduce cloning time you can use
  `--depth 1` option). This step is needed only for one of the two integration approaches: when
  using a pre-installed directory rather than `FetchContent`.
    ```sh
     $ git clone https://github.com/alanxz/rabbitmq-c.git
    ```

### Building and installing

The KasperskyOS-adapted RabbitMQ-C library is built using the CMake build system, which is provided
in the KasperskyOS Community Edition SDK. When you develop a KasperskyOS-based solution, use the
[recommended structure of project directories](https://kas.pr/9zph)
to simplify the use of CMake scripts.

This solution uses the dynamic variant of the KasperskyOS-adapted RabbitMQ-C library. To build a
static variant of the library, use the `initialize_platform` command with the `FORCE_STATIC`
parameter in the root `CMakeLists.txt`. For additional details regarding this command, please refer
to the [platform library](https://kas.pr/8r3i).

To build and install both static and dynamic versions of the library, execute the following
commands:
```sh
$ cmake -B build \
        -D CMAKE_TOOLCHAIN_FILE="${KOSCEDIR}/toolchain/share/toolchain-aarch64-kos.cmake" \
        -D CMAKE_INSTALL_PREFIX=~/.local/share/kos/$(basename $KOSCEDIR)/sysroot-aarch64-kos
$ cmake --build build -j`nproc` --target install
```
Set `CMAKE_INSTALL_PREFIX` to your preferred installation path. For compatibility with the
[amqp_publisher](../../amqp_publisher/README.md) and [amqp_consumer](../../amqp_consumer/README.md)
examples, we recommend setting the `CMAKE_INSTALL_PREFIX` to
`~/.local/share/kos/$(basename $KOSCEDIR)/sysroot-aarch64-kos`.

#### CMake input files

[./CMakeLists.txt](CMakeLists.txt)—CMake commands for including the KasperskyOS-adapted RabbitMQ-C
library in the project. This module implements the logic for the two supported integration methods.
It checks for a user-provided source directory via the `RABBITMQ_SOURCE_DIR` variable, and
automatically fetches the specific version of the RabbitMQ-C library using the CMake module
`FetchContent`.

## Usage

To integrate the library in a KasperskyOS-based solution, there are two recommended options: using
the previously installed RabbitMQ-C library, or using the CMake module `FetchContent`.

For practical guidance, see the [amqp_publisher](../../amqp_publisher/README.md) and
[amqp_consumer](../../amqp_consumer/README.md) examples, which demonstrate both approaches.

When using the pre-installed RabbitMQ-C library, follow these steps:

1. Add the built library to your solution using the CMake command
 `find_package(rabbitmq-c REQUIRED)`.
1. Link the library to your program using the CMake command `target_link_libraries()`.
1. Add the required header files to your source code to enable calls to library functions:
   ```c
   #include <rabbitmq-c/amqp.h>
   #include <rabbitmq-c/framing.h>
   #include <rabbitmq-c/tcp_socket.h>
   ...

When using the `FetchContent` CMake module, refer to the build configuration in the example projects
mentioned above.

[⬆ Back to Top](#Table-of-contents)

© 2026 AO Kaspersky Lab
