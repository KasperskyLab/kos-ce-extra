# Using WPA supplicant client

This solution is an example of using the `wpa_supplicant` client library with KasperskyOS on
Raspberry Pi 4 Model B and Radxa Rock 3A hardware platforms. The example demonstrates connecting to
a wireless network and making a test connection to a server:

* Scan the network and print the result.
* Connect to the configured SSID (Service Set Identifier).
* Attempt to create a TCP connection to an external host server and send the message `Hello from
  KasperskyOS` to it.
* Disconnect from the SSID.

The example uses the static variant of the `wpa_supplicant` client library (the
`initialize_platform` command with the `FORCE_STATIC` parameter). For additional details regarding
this command, please refer to the
[platform library](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&&customization=KCE&helpid=cmake_platform_lib).

For additional details on KasperskyOS, including its limitations and known issues, please refer to the
[KasperskyOS Community Edition Online Help](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=community_edition).

## Table of contents
- [Using WPA supplicant client](#using-wpa-supplicant-client)
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

* `Client`—Program that retrieves a list of available wireless networks and makes a test connection
  to a server.
* `WpaSupplicant`—System program that provides a secure connection to wireless access points.
* `VfsNet`—System program that supports network protocols.
* `Dhcpcd`—System program that implements a DHCP client, which gets network interface parameters
  from an external DHCP server in the background and passes them to a virtual file system.
* `EntropyEntity`—System program that implements random number generation.
* `DCM`—System program that lets you dynamically create IPC channels.
* `DNetSrvBrcmfmac`—Driver for the Broadcom WiFi chip.
* `DNetSrv`—Driver for working with network cards.

When you build the example for the target hardware platform, platform-specific drivers are
automatically included in the solution:

* `BSP`—Hardware platform support package (Board Support Package). Provides cross-platform
configuration of peripherals for the Radxa ROCK 3A and Raspberry Pi 4 B.
* `GPIO`—GPIO support driver for the Radxa ROCK 3A and Raspberry Pi 4 B.
* `PinCtrl`—Low-level pin multiplexing (pinmux) configuration driver for the Radxa ROCK 3A.
* `Bcm2711MboxArmToVc`—Driver for working with the VideoCore (VC6) coprocessor via mailbox
technology for Raspberry Pi 4 B.

[⬆ Back to Top](#Table-of-contents)

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
1. [Prepare the WiFi driver firmware](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=get_wifi_firmware)
  for operation.
1. Replace the parameter values located in the
  [`./resources/hdd/etc/client.conf`](resources/hdd/etc/client.conf) configuration file with your
  values:

    * `ssid`—WiFi access point name for connection;
    * `psk`—WiFi access point password for connection;
    * `serverIp`—Server IP address inside WiFi access point subnet;
    * `serverPort`—Server port inside WiFi access point subnet.
1. Start a listening socket on an external server inside the WiFi access point subnet.
For example, use this command:
    ```sh
    nc -lvv <serverIp> <serverPort>
    ```

[⬆ Back to Top](#Table-of-contents)

### Building and running the example

The example is built using the CMake build system, which is provided in the KasperskyOS Community
Edition SDK.

To build the example to run on the target hardware platform, go to the directory with the example
and run the following commands:
```sh
$ cmake -B build -D CMAKE_FIND_ROOT_PATH="${HOME}/.local/share/kos;${KOSCEDIR}/sysroot-aarch64-kos" -D CMAKE_TOOLCHAIN_FILE="$KOSCEDIR/toolchain/share/toolchain-aarch64-kos.cmake"
$ cmake --build build --target {kos-image|sd-image}
```
where:

* `kos-image` creates a KasperskyOS-based solution image that includes the example;
* `sd-image` creates a file system image for a bootable SD card.

After a successful build, the `kos-image` solution image will be located at the `./build/einit`
directory. The `hdd.img` bootable SD card image will be located at the `./build` directory.

To run the example on the target hardware platform:

1. Connect the SD card to the computer and copy the bootable SD card image to the SD card using the
  command:
   ```sh
   $ sudo dd bs=64k if=build/hdd.img of=/dev/sd[X] conv=fsync
   ```

   where `[X]` is the final character in the name of the SD card block device.

1. Connect the bootable SD card to the board.
1. Supply power to the board and wait for the example to run.

You can also use an alternative option to prepare and run the example:

1. Prepare the board and a bootable SD card to run the example by following the instructions
in the [Preparing Raspberry Pi 4](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&&customization=KCE&helpid=preparing_sd_card_rpi) or [Preparing Radxa ROCK 3A](https://support.kaspersky.com/help/KCE/1.4/en-US/preparing_sd_card_radxa.htm).
1. Run the example by following the instructions in the
[KasperskyOS Community Edition Online Help](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&&customization=KCE&helpid=running_sample_programs_rpi)

[⬆ Back to Top](#Table-of-contents)

#### CMake input files

[./client/CMakeLists.txt](client/CMakeLists.txt)—CMake commands for building the `Client` program.

[./einit/CMakeLists.txt](einit/CMakeLists.txt)—CMake commands for building the `Einit` program and
the solution image.

[./CMakeLists.txt](CMakeLists.txt)—CMake commands for building the solution.

## Usage

After running the example, the network parameters and the results of any actions performed should be
displayed in standard output. Assuming you've started to listen on the chosen port on your server
(i.e. `nc -lvvp 5555`), the client will connect to it and send the `Hello from KasperskyOS` message.

> [!TIP]
> If the list of networks is empty and you have large amount of visible networks in your
> area, you may need to increase the scan duration. To do this, update `scanTime` value in
> [./client/src/connection_manager.cpp](client/src/connection_manager.cpp).

[⬆ Back to Top](#Table-of-contents)

© 2026 AO Kaspersky Lab
