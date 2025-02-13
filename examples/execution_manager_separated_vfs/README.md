# Example of using the ExecutionManager component API

The KasperskyOS Community Edition SDK includes the ExecutionManager component.
The ExecutionManager component provides a C++ interface for creating, starting,
and stopping processes in solutions that are based on KasperskyOS. This example
demonstrates a reference use of the ExecutionManager component API.

For more information about the ExecutionManager API, see the following topics:
[ExecutionManager component](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.3&customization=KCE&helpid=execmgr_component)
and
[Starting processes using the system program ExecutionManager](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.3&customization=KCE&helpid=app_static_start)
in the KasperskyOS Community Edition Developer's Guide.

For additional details on KasperskyOS, including its limitations and known issues, please refer to the
[KasperskyOS Community Edition Online Help](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.3&customization=KCE&helpid=community_edition).

# Table of contents
- [Example of using the ExecutionManager component API](#example-of-using-the-executionmanager-component-api)
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
- [Trademarks](#trademarks)

## Solution overview

### List of programs

* `Client`—Program that manages two applications using IPC to interact with the ExecutionManager
component
* `ExecMgrEntity`—Program that provides an interface for creating, starting, and stopping processes
* `BlobContainer`—Program that loads dynamic libraries used by other programs into shared memory
* `DCM`—Program for dynamic creation of IPC channels
* `NameServer`—Program that provides an interface for dynamically creating IPC channels
* `RAMDisk`—Block device driver of a virtual drive in RAM
* `EntropyEntity`—Random number generator
* `VfsRamFs`—Program that supports RamFS file system

To make the client call the interface methods provided by the ExecutionManager component, the header
file `component/execution_manager/kos_ipc/execution_manager_proxy.h` is included in the
[`./client/src/main.cpp`](client/src/main.cpp) file.

### Initialization description

<details><summary>Statically created IPC channels</summary>

* `client.Client` → `kl.core.NameServer`
* `client.Client` → `kl.bc.BlobContainer`
* `client.Client` → `execution_manager.ExecMgrEntity`
* `kl.bc.BlobContainer` → `kl.VfsRamFs`
* `execution_manager.ExecMgrEntity` → `kl.bc.BlobContainer`
* `kl.core.NameServer` → `kl.bc.BlobContainer`
* `kl.VfsRamFs` → `kl.drivers.RAMDisk`
* `kl.VfsRamFs` → `kl.EntropyEntity`
* `kl.VfsRamFs` → `kl.bc.BlobContainer`
* `kl.drivers.RAMDisk` → `kl.bc.BlobContainer`
* `kl.EntropyEntity` → `kl.bc.BlobContainer`

</details>

The [`./einit/src/init.yaml.in`](einit/src/init.yaml.in) template is used to automatically generate
a part of the solution initialization description file `init.yaml`. For more information about the
`init.yaml.in` template file, see the
[KasperskyOS Community Edition Online Help](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.3&customization=KCE&helpid=cmake_yaml_templates).

### Security policy description

The [`./einit/src/security.psl.in`](einit/src/security.psl.in) template is used to automatically
generate a part of the `security.psl` file using CMake tools. The `security.psl` file contains a part
of a solution security policy description. For more information about the `security.psl` file, see
[Describing a security policy for a KasperskyOS-based solution](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.3&customization=KCE&helpid=ssp_descr).

[⬆ Back to Top](#Table-of-contents)

## Getting started

### Prerequisites

1. To install [KasperskyOS Community Edition SDK](https://os.kaspersky.com/development/) and run
examples on QEMU or on a hardware platform, make sure you meet all the
[System requirements](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.3&customization=KCE&helpid=system_requirements),
which are listed in the KasperskyOS Community Edition Developer's Guide.
1. [Install](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.3&customization=KCE&helpid=sdk_install_and_remove)
KasperskyOS Community Edition SDK. You can download the latest version of the KasperskyOS Community
Edition for free from [os.kaspersky.com](https://os.kaspersky.com/development/). The minimum
required version of KasperskyOS Community Edition SDK is 1.3. For more information, see
[System requirements](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.3&customization=KCE&helpid=system_requirements).
1. Copy source files to your project directory.

### Building and running the example

The example is built using the CMake build system, which is provided in the KasperskyOS Community
Edition SDK.

To build the example, run the [`./cross-build.sh`](cross-build.sh) script. There are environment
variables that affect the build of the example:

* `SDK_PREFIX` specifies the path to the installed version of the KasperskyOS Community Edition SDK.
If the `-s` option is not specified when the `cross-build.sh` script has been run, it is necessary
to set the value of the `SDK_PREFIX` environment variable.
* `TARGET` specifies the target platform. (Currently only the `aarch64-kos` platform is supported.)

Syntax for using the `cross-build.sh` script:

`$ ./cross-build.sh [-h] [-l] [-t TARGETS] [-s PATH] [-b PATH]`,

where:

* `-h, --help`

  Help text.
* `-l, --list`

  List of all CMake targets which are available for building.
* `-t, --target TARGETS`

  List of CMake targets that you want to build. Enclose the entire list of targets in quotation
marks and separate the individual targets within the quotation marks with a space character. If not
specified, the default target `sim` will be built.
* `-s, --sdk-path PATH`

  Path to the installed version of the KasperskyOS Community Edition SDK. The path must be set using
either the value of the `SDK_PREFIX` environment variable or the `-s` option. The value specified in
the `-s` option takes precedence over the value of the `SDK_PREFIX` environment variable.
* `-b, --build PATH`

  Path to the generated CMake build directory. If not specified, the default path `./build` will be used.

For example, review the following command:
```sh
$ SDK_PREFIX=/opt/KasperskyOS-Community-Edition-<version> ./cross-build.sh
```
The command builds the example and runs the KasperskyOS-based solution image on QEMU. The solution
image is based on the SDK found in the `/opt/KasperskyOS-Community-Edition-<version>` path, where
`version` is the latest version number of the KasperskyOS Community Edition SDK.

#### QEMU

Running `cross-build.sh` creates a KasperskyOS-based solution image that includes the example. The
`kos-qemu-image` solution image is located in the `./<build_path>/einit` directory, where
`build_path` is the path to the generated CMake build directory.

The `cross-build.sh` script both builds the example on QEMU and runs it.

#### Hardware

Running `cross-build.sh` creates a KasperskyOS-based solution image that includes the example and a
bootable SD card image for Raspberry Pi 4 B or Radxa ROCK 3A. The `kos-image` solution image is located in the
`./<build_path>/einit` directory. The `hdd.img` bootable SD card image is located in the
`./<build_path>` directory.

1. To copy the bootable SD card image to the SD card, connect the SD card to the computer and
run the following command:

  `$ sudo dd bs=64k if=build/hdd.img of=/dev/sd[X] conv=fsync`,

  where `[X]` is the final character in the name of the SD card block device.

1. Connect the bootable SD card to the Raspberry Pi 4 B or Radxa ROCK 3A.
1. Supply power to the Raspberry Pi 4 B or Radxa ROCK 3A and wait for the example to run.

You can also use an alternative option to prepare and run the example:

1. Prepare the required hardware platform and bootable SD card for it by following the instructions:
    * [Raspberry Pi 4 B](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.3&customization=KCE&helpid=preparing_sd_card_rpi)
    * [Radxa ROCK 3A](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.3&customization=KCE&helpid=preparing_sd_card_radxa)
1. Run the example as described in the
[KasperskyOS Community Edition Online Help](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.3&customization=KCE&helpid=running_sample_programs_rpi)

#### CMake input files

When you develop a KasperskyOS-based solution, use the
[recommended structure of project directories](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.3&customization=KCE&helpid=cmake_using_sdk_cmake)
to simplify usage of CMake scripts.

The `CMakeLists.txt` files use standard CMake syntax and scripts from the `platform`, `nk`,
`install`, and `image` SDK libraries.

[`./сlient/CMakeLists.txt`](сlient/CMakeLists.txt)—CMake commands for building the `Сlient` program.
Link the client executable file to the client proxy library of ExecutionManager component by adding the
following command to the CMake file for building the client:
```
target_link_libraries (${PROGRAM_NAME} ${vfs_CLIENT_LIB}
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
                                NK_MODULE_NAME ${PROJECT_NAME}
                                MAIN_CONN_NAME ${PROGRAM_NAME}
                                ROOT_PATH      "/"
                                VFS_CLIENT_LIB ${vfs_CLIENT_LIB})
```
For a description of the parameters, see
[Starting processes using the system program ExecutionManager](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.3&customization=KCE&helpid=app_static_start).

[`./CMakeLists.txt`](CMakeLists.txt)—CMake commands for building the solution. In this file, the
following commands are required to connect the ExecutionManager component:
```
find_package (execution_manager REQUIRED)
include_directories (${execution_manager_INCLUDE})
add_subdirectory (execution_manager)
```

[⬆ Back to Top](#Table-of-contents)

## Usage

[Build and run](#building-and-running-the-example) the example. After running the example, the following
actions will be executed:

1. The KasperskyOS kernel runs the `Einit` initialization process.
1. The `Einit` initializes static IPC channels and runs all processes except two
applications which are run by the `Client`.
1. The `Client` executes the following actions:
  1. Publishes the provided endpoints on the name server.
  1. Starts two applications.
  1. Waits for notifications from the applications.
  1. Stops both applications.
1. On successful completion of work, the client and applications print messages to standard output.
The expected output is contained in the [`./expected_output.txt`](expected_output.txt) file.

# Trademarks

Registered trademarks and endpoint marks are the property of their respective owners.

Raspberry Pi is a trademark of the Raspberry Pi Foundation.

[⬆ Back to Top](#Table-of-contents)

© 2025 AO Kaspersky Lab
