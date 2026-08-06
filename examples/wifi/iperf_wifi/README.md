# Using iperf with WiFi

This example demonstrates the deployment of an iperf3 server accessible over a wireless network
interface, through integration of the `iperf3` network performance testing application with the
`wpa_supplicant` wireless authentication client and the `dnet_brcmfmac` WiFi driver with KasperskyOS
on Raspberry Pi 4 Model B and Radxa Rock 3A hardware platforms.

The example uses the static variant of the `wpa_supplicant` client library (the
`initialize_platform` command with the `FORCE_STATIC` parameter). For additional details regarding
this command, please refer to the
[platform library](https://kas.pr/8r3i).

For additional details on KasperskyOS, including its limitations and known issues, please refer to the
[KasperskyOS Community Edition Online Help](https://kas.pr/si81).

## Table of contents
- [Using iperf with WiFi](#using-iperf-with-wifi)
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

* `Server`—Program that runs the iperf3 server and implements network performance testing over WiFi.
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
[init.yaml.in template](https://kas.pr/nb5m).

### Security policy description

The [`./einit/src/security.psl`](einit/src/security.psl) file describes the security policy of the
solution. The declarations in the PSL file are provided with comments that explain the purpose of
these declarations. For more information about the `security.psl` file, see
[Describing a security policy for a KasperskyOS-based solution](https://kas.pr/jm1v).

## Getting started

### Prerequisites

1. Confirm that your host system meets all the
[System requirements](https://kas.pr/u87d)
listed in the KasperskyOS Community Edition Developer's Guide.
1. [Install](https://kas.pr/ce73)
the KasperskyOS Community Edition SDK version 1.4. You can download it for free from
[os.kaspersky.com](https://kas.pr/4mro).
1. Copy the source files of this example to your local project directory.
1. Set up the build environment by sourcing the SDK setup script in your terminal session:
   ```sh
   source /opt/KasperskyOS-Community-Edition-<platform>-<version>/common/set_env.sh
   ```
1. [Build the necessary drivers](https://kas.pr/ia13)
from source only if you intend to run this example on Radxa ROCK 3A hardware. This step is not
required for Raspberry Pi 4 B.
1. [Prepare the WiFi driver firmware](https://kas.pr/s77q)
  for operation.
1. Replace the parameter values located in the
  [`./resources/hdd/etc/wpa_supplicant.conf`](resources/hdd/etc/wpa_supplicant.conf)
  configuration file with your values:

   * `ssid`—WiFi access point name for connection;
   * `psk`—WiFi access point password for connection.

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

1. Prepare the required hardware platform and a bootable SD card to run the example by following the
instructions:

   - [Raspberry Pi 4 B](https://kas.pr/i8mg).
   - [Radxa ROCK 3A](https://kas.pr/21n7).
1. Run the example by following the instructions in the
[KasperskyOS Community Edition Online Help](https://kas.pr/9oa7).

[⬆ Back to Top](#Table-of-contents)

#### CMake input files

[./server/CMakeLists.txt](server/CMakeLists.txt)—CMake commands for building the `Server` program.

[./einit/CMakeLists.txt](einit/CMakeLists.txt)—CMake commands for building the `Einit` program and
the solution image.

[./CMakeLists.txt](CMakeLists.txt)—CMake commands for building the solution.

## Usage

After you have [built and run](#building-and-running-the-example) the example, the solution will
execute the following sequence:

1. Establish a wireless connection using `wpa_supplicant` to connect to the configured network.
1. Obtain an IP address via DHCP on the wireless interface `wl0`.
1. Start the iperf3 server, which begins listening for performance test connections on the default
  port `5201`.

During execution, network interface details and assigned IP addresses are displayed in the standard
error output upon successful connection.

Once the server is running, you can connect to it using the iperf3 client:

1. Identify the server's IP address from the connection output.
1. Run the iperf3 client on the host machine with the following command, replacing `SERVER_IP_ADDRESS` with this
   address:
   ```
   $ iperf3 -c <SERVER_IP_ADDRESS> -t5 -b20M --bidir --udp
   ```

The client output displays test results including transfer speeds, bitrates, and performance
metrics.

[⬆ Back to Top](#Table-of-contents)

© 2026 AO Kaspersky Lab
