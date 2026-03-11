# Using the PackageManager component API

This example demonstrates how to work with
[KPA packages](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=kpa_package)
in a KasperskyOS-based solution:

* Packing source files into the KPA package using the
  [`cas-pack`](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=tools_cas_pack)
  tool.
* Outputting the KPA package info using the
  [`cas-inspect`](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=tools_cas_inspect)
  tool.
* Pre-installing the KPA package into a built image of a KasperskyOS-based solution using the
  [`cas-pm`](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=tools_cas_pm)
  tool.
* Getting data on a
  [KPA package manifest](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=kpa_manifest)
  using the
  [PackageManager](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=packmgr_component)
  component.
* Starting a process from an executable file installed from the KPA package using the
  [ExecutionManager](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=execmgr_component)
  component.

How to work with the ExecutionManager component, see the
[`execution_manager_separated_vfs example`](../execution_manager_separated_vfs/README.md).

For additional details on KasperskyOS, including its limitations and known issues, please refer to
the [KasperskyOS Community Edition Online Help](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=community_edition).

# Table of contents
- [Using the PackageManager component API](#using-the-packagemanager-component-api)
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

* `Application`—Program that reads the content of a text resource file and outputs its content to
standard error output.
* `Launcher`—Program that starts the `Application` program installed from the KPA package. For this
program to call the interface methods provided by the PackageManager component, the header file
`component/package_manager/kos_ipc/package_manager_proxy.h` must be included in the
[`./launcher/src/main.cpp`](launcher/src/main.cpp) file.
* `PkgMgrEntity`—System program that provides an interface for managing KPA packages.
* `ExecMgrEntity`—System program that provides an interface for creating, starting, and stopping
  processes.
* `BlobContainer`—System program that is designed to load binary data into memory and is used by the
  `ExecMgrEntity` program to start processes.
* `DCM`—System program for dynamic creation of IPC channels.
* `EntropyEntity`—System program that implements random number generation.
* `VfsSdCardFs`—System program that supports the file system of SD cards.
* `SDCard`—SD card driver.

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
1. Copy the source files of this example to your local project directory.
1. Set up the build environment by sourcing the SDK setup script in your terminal session:
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

To build the example to run on the target hardware platform, go to the directory with the example and
run the following commands:
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

[`./application/CMakeLists.txt`](application/CMakeLists.txt)—CMake commands for
building the `Application` program.

[`./einit/CMakeLists.txt`](einit/CMakeLists.txt)—CMake commands for building the `Einit` program and
the solution image.

[`./package_manager/CMakeLists.txt`](package_manager/CMakeLists.txt)—CMake commands for building the
`PkgMgrEntity` program.

[`./execution_manager/CMakeLists.txt`](execution_manager/CMakeLists.txt)—CMake commands for building
the `ExecMgrEntity` program.

[`./kpa_package/CMakeLists.txt`](kpa_package/CMakeLists.txt)—CMake commands for packing the
`Application` program source files into the KPA package, getting information about the contents of
the KPA package, and installs this package into a KasperskyOS-based solution image.

[`./launcher/CMakeLists.txt`](launcher/CMakeLists.txt)—CMake commands for building the `Launcher`
program, that starts the `Application` program with the ExecutionManager component.

[`./CMakeLists.txt`](CMakeLists.txt)—CMake commands for building the solution.

## Usage

[Build and run](#building-and-running-the-example) the example. After running the example, the
following actions will be executed:

1. The KasperskyOS kernel runs the `Einit` initialization process.
1. `Einit` initializes static IPC channels and runs all processes except the `Application` program.
1. The `Launcher` program starts the `Application` program.
1. The `Application` program reads the content of a text resource file and outputs them to standard
error output.
1. On successful completion of work, programs print messages to standard error output. The expected
output is contained in the [`./expected_output.txt`](expected_output.txt) file.

[⬆ Back to Top](#Table-of-contents)

© 2026 AO Kaspersky Lab
