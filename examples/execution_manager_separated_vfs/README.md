# Using the ExecutionManager component API

This example demonstrates how to use the ExecutionManager component API in a KasperskyOS-based
solution. The ExecutionManager component is included in the KasperskyOS Community Edition SDK and
provides a C++ interface for programmatically creating, starting, and stopping processes at runtime.

For more information about the ExecutionManager API, see the following topics:
[ExecutionManager component](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=execmgr_component)
and
[Starting processes using the system program ExecutionManager](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=app_static_start)
in the KasperskyOS Community Edition Developer's Guide.

For additional details on KasperskyOS, including its limitations and known issues, please refer to
the [KasperskyOS Community Edition Online Help](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=community_edition).

# Table of contents
- [Using the ExecutionManager component API](#using-the-executionmanager-component-api)
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

* `Client`—Program that uses IPC to manage two applications via the ExecutionManager component.
* `ExecMgrEntity`—System program that manages the life cycle of processes via IPC mechanisms.
* `BlobContainer`—System program that is designed to load binary data into memory and is used by the
`ExecMgrEntity` program to start processes.
* `NameServer`—System program that provides naming and discovery interface for locating and
connecting to distributed components.
* `SDCard`—SD card driver.
* `EntropyEntity`—System program that implements random number generation.
* `VfsSdCardFs`—System program that supports the file system of SD cards.
* `DCM`—System program that lets you dynamically create IPC channels.

When you build the example for the target hardware platform, platform-specific drivers are
automatically included in the solution:

* `BSP`—Hardware platform support package (Board Support Package). Provides cross-platform
configuration of peripherals for the Radxa ROCK 3A and Raspberry Pi 4 B.
* `GPIO`—GPIO support driver for the Radxa ROCK 3A.
* `PinCtrl`—Low-level pin multiplexing (pinmux) configuration driver for the Radxa ROCK 3A.
* `Bcm2711MboxArmToVc`—Driver for working with the VideoCore (VC6) coprocessor via mailbox
technology for Raspberry Pi 4 B.

To make the client call the interface methods provided by the ExecutionManager component, the header
file `component/execution_manager/kos_ipc/execution_manager_proxy.h` is included in the
[`./client/src/main.cpp`](client/src/main.cpp) file.

### Initialization description

The solution initialization description file named `init.yaml` is generated during the solution
build process based on the [`./einit/src/init.yaml.in`](einit/src/init.yaml.in) template.
The macros in `@INIT_*@`‌ ‌format contained in the template are
automatically expanded in the resulting `init.yaml` file. For more details, refer to
[init.yaml.in template](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=cmake_yaml_templates).

### Security policy description

The [`./einit/src/security.psl`](einit/src/security.psl) file describes the security policy of the
solution. The declarations in the PSL file are provided with comments that explain the purpose of
these declarations. For more information about the `security.psl` file, see
[Describing a security policy for a KasperskyOS-based solution](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=ssp_descr).

[⬆ Back to Top](#Table-of-contents)

## Getting started

### Prerequisites

1. Confirm that your host system meets all the
[System requirements](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=system_requirements)
listed in the KasperskyOS Community Edition Developer's Guide.
1. [Install](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=sdk_install_and_remove)
the KasperskyOS Community Edition SDK version 1.4. You can download it for free from
[os.kaspersky.com](https://os.kaspersky.com/development/).
1. Copy the source files of this `execution_manager_separated_vfs` example to your local project
directory.
1. Source the SDK setup script to configure the build environment. This exports the `KOSCEDIR`
  environment variable, which points to the SDK installation directory:
   ```sh
   source /opt/KasperskyOS-Community-Edition-<platform>-<version>/common/set_env.sh
   ```
1. [Build the necessary drivers](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=building_radxa_drivers)
from source only if you intend to run this example on Radxa ROCK 3A hardware. This step is not
required for QEMU or Raspberry Pi 4 B.

### Building and running the example

The example is built using the CMake build system, which is provided in the KasperskyOS Community
Edition SDK.

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

After a successful build, the `kos-qemu-image` solution image will be located in the `./build/einit`
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

   - [Raspberry Pi 4 B](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=preparing_sd_card_rpi).
   - [Radxa ROCK 3A](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=preparing_sd_card_radxa).
1. Run the example by following the instructions in the
[KasperskyOS Community Edition Online Help](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=running_sample_programs_rpi).

[⬆ Back to Top](#Table-of-contents)

#### CMake input files

When you develop a KasperskyOS-based solution, use the
[recommended structure of project directories](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=cmake_using_sdk_cmake)
to simplify the use of CMake scripts.

[`./client/CMakeLists.txt`](client/CMakeLists.txt)—CMake commands for building the `Client` program.
Link the client executable file to the client proxy library of ExecutionManager component by adding
the following command to the CMake file for building the client:
```
target_link_libraries (${PROGRAM_NAME} vfs::client
                                       ${execution_manager_EXECMGR_PROXY})
```

[`./einit/CMakeLists.txt`](einit/CMakeLists.txt)—CMake commands for building the `Einit` program and
the solution image.

[`./execution_manager/CMakeLists.txt`](execution_manager/CMakeLists.txt)—CMake commands for building
the `ExecMgrEntity` program. The ExecutionManager component is provided in the SDK as a set of
static libraries and header files, and is built for a specific solution by using the CMake command
`create_execution_manager_entity()` from the CMake library `execution_manager`:
```
include (execution_manager/create_execution_manager_entity)
create_execution_manager_entity(ENTITY         ${PROGRAM_NAME}
                                NK_MODULE_NAME "execution_manager"
                                MAIN_CONN_NAME ${PROGRAM_NAME}
                                ROOT_PATH      "/"
                                VFS_CLIENT_LIB vfs::client)
```
For a description of the parameters, see
[Starting processes using the system program ExecutionManager](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=app_static_start).

[`./CMakeLists.txt`](CMakeLists.txt)—CMake commands for building the solution. In this file, the
following commands are required to connect the ExecutionManager component:
```
find_package (execution_manager REQUIRED)
include_directories (${execution_manager_INCLUDE})
add_subdirectory (execution_manager)
```

## Usage

[Build and run](#building-and-running-the-example) the example. After running the example, the
following actions will be executed:

1. The KasperskyOS kernel runs the `Einit` initialization process.
1. The `Einit` initializes static IPC channels and runs all processes except two
applications which are run by the `Client`.
1. The `Client` executes the following actions:
   1. Publishes the provided endpoints on the name server.
   1. Starts two applications.
   1. Waits for notifications from the applications.
   1. Stops both applications.
1. On successful completion of work, the client and applications print messages to standard output.
The expected output can be found in the [./expected_output.txt](expected_output.txt) file.

[⬆ Back to Top](#Table-of-contents)

© 2026 AO Kaspersky Lab
