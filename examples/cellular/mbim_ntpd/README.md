# Internet connectivity via MBIM
This example demonstrates the use of the MBIM (Mobile Broadband Interface Model) network driver to
establish an internet connection: it first activates the modem's data session via the KasperskyOS
MBIM API, then configures this connection as the default network gateway, and finally synchronizes
the system time with external NTP servers.

For additional details on KasperskyOS, including its limitations and known issues, please refer to
the [KasperskyOS Community Edition Online Help](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=community_edition).

# Table of contents
- [Internet connectivity via MBIM](#internet-connectivity-via-mbim)
- [Table of contents](#table-of-contents)
  - [Solution overview](#solution-overview)
    - [List of programs](#list-of-programs)
    - [Initialization description](#initialization-description)
    - [Security policy description](#security-policy-description)
  - [Getting started](#getting-started)
    - [Prerequisites](#prerequisites)
    - [Building and running the example](#building-and-running-the-example)
      - [CMake input files](#cmake-input-files)
    - [Usage](#usage)

## Solution overview

### List of programs

* `Client`—Program that establishes a mobile internet connection through the MBIM driver using the
specified Access Point Name (APN). It also initializes the system time by setting it to the
specified date and, after connection, verifies successful time synchronization with NTP servers.
* `DCM`—System program for dynamic creation of IPC channels.
* `EntropyEntity`—System program that implements random number generation.
* `Ntpd`—System program that implements an NTP client that receives time parameters from external
  NTP servers in the background and forwards them to the KasperskyOS kernel.
* `VfsNet`—System program that supports network protocols.
* `DNetSrv`—Network card driver.
* `DNetSrvMbim`—Network driver that implements the MBIM protocol for mobile network connectivity.
* `USB`—Driver for managing all types of USB devices.

When you build the example for the target hardware platform, platform-specific drivers are
automatically included in the solution:

* `BSP`—Hardware platform support package (Board Support Package). Provides cross-platform
configuration of peripherals for the Radxa ROCK 3A.
* `GPIO`—GPIO support driver for the Radxa ROCK 3A.
* `PinCtrl`—Low-level pin multiplexing (pinmux) configuration driver for the Radxa ROCK 3A.
* `PCIE`—PCIe bus driver for Raspberry Pi 4 B.
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
required for Raspberry Pi 4 B.
1. Prepare your LTE modem hardware by ensuring the following:
   * Insert a SIM card without a PIN code into the modem.
   * Connect the antenna to the modem's `MAIN` port.
   * Connect an LTE modem with MBIM support to your board.

> [!TIP]
> The `Waveshare SIM7600X 4G HAT` is the recommended LTE modem for use with hardware platforms
> supported by the KasperskyOS Community Edition SDK. Connection to your hardware platform's 40-pin
> header is not required.
>
> This modem's onboard UART adapter is limited to `1` MBaud. You may need to use a dedicated
> USB-UART adapter if your hardware platform requires a higher baud rate.

> [!NOTE]
> Update the APN settings in the [`./client/src/client.c`](client/src/client.c) source file to match
> those required by your cellular provider.

### Building and running the example
The example is built using the CMake build system, which is provided in the KasperskyOS Community
Edition SDK.

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

> [!IMPORTANT]
> In rare cases modem firmware may hang on boot and require a reboot. Make sure the hardware is
> powered with a sufficient power supply to minimize reliability issues.

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

[`./einit/CMakeLists.txt`](einit/CMakeLists.txt)—CMake commands for building the `Einit` program and
the solution image.

[`./CMakeLists.txt`](CMakeLists.txt)—CMake commands for building the solution.

### Usage

[Build and run](#building-and-running-the-example) the example. During execution, the programs print
status messages to standard error output. The expected output is contained in the
[`./expected_output.txt`](expected_output.txt) file.

[⬆ Back to Top](#Table-of-contents)

© 2026 AO Kaspersky Lab
