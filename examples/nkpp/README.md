# nkppmeta compiler usage example

The KasperskyOS SDK includes the nkppmeta compiler for generating transport code in C++. The
nkppmeta compiler lets you generate transport C++ proxy objects and stubs for use by both a client
and a server.

Proxy objects are used by the client to pack the parameters of the called method into an IPC
request, execute the IPC request, and unpack the IPC response. Stubs are used by the server to
unpack the parameters from the IPC request, dispatch the call to the appropriate method
implementation, and pack the IPC response. For more information, please refer to the
[Transport code in C++](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=cpp_proxy_stubs).

For additional details on KasperskyOS, including its limitations and known issues, please refer to
the [KasperskyOS Community Edition Online Help](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=community_edition).

# Table of contents
- [nkppmeta compiler usage example](#nkppmeta-compiler-usage-example)
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

* `Client`—Program that interacts with endpoints provided by the server.
* `Server`—Program that contains several nested components and provides endpoints for the
`example.Animal` interface.
* `DCM`—System program that lets you dynamically create IPC channels.

To make the client call an interface method provided by the server, the following steps are taken in
the [`./client/src/main.cpp`](client/src/main.cpp) file:

1. The generated client description header `Client.edl.cpp.h` is included.
1. The generated header files of the descriptions of the utilized interfaces `*.idl.cpp.h` are
included. In this example, only `Animal.idl.cpp.h` is included.
1. The `kosipc/make_application.h` and corresponding `kosipc/connect_*.h` headers are included.
In this example, only `kosipc/connect_static_channel.h` is included.
1. The application object is initialized with the `kosipc::MakeApplicationAutodetect()` function.
1. The proxy object for the endpoint to be used is initialized with the `Application::MakeProxy()`
function by selecting one of the `Connect*()` functions as an argument. In this example, the
`ConnectStaticChannel()` function is used because the connection to the endpoint uses the static
channel described in the `init.yaml` file.
1. The corresponding method of the proxy object is called.

To make the server provide endpoints to other processes, the following steps are taken in the
[`./server/src/main.cpp`](server/src/main.cpp) file:

1. The generated server description header `Server.edl.cpp.h` that contains the component structure
of the server including all endpoints is included.
1. The `kosipc/make_application.h` and corresponding `kosipc/serve_*.h` headers are included. In
this example, only `kosipc/serve_static_channel.h` is included.
1. Classes that contain implementations of the interfaces (here it is only `Animal`) that this
program and its components provide as endpoints are created.
1. The application object is initialized with the `kosipc::MakeApplicationAutodetect()` function.
1. The `kosipc::components::Root` structure which describes the component structure and interfaces
of program endpoints is initialized.
1. The fields of the `Root` structure are associated with the objects that implement the
corresponding endpoints. The fields of the `Root` structure repeat the hierarchy in the CDL and EDL
files.
1. The incoming request processing cycle is created by calling the `Application::MakeEventLoop()`
function and selecting one of the `Serve*()` functions as an argument. In this example the
`ServeStaticChannel()` function is used because server maintains static channel described in
`init.yaml` file. You need to pass the channel name that you want to maintain and the component
structure (`Root`) that contains all the endpoint implementations into the `ServeStaticChannel()`
function.
1. The incoming IPC message dispatch cycle is run by using the `Run()` method of the
`kosipc::EventLoop` object.

### Initialization description

The solution initialization description file named `init.yaml` is generated during the solution
build process based on the [`./einit/src/init.yaml.in`](einit/src/init.yaml.in) template. Macros in
the `@INIT_*@`‌ ‌format contained in the template are automatically expanded in the resulting
`init.yaml` file. For more details, refer to
[init.yaml.in template](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=cmake_yaml_templates).

### Security policy description

The [`./einit/src/security.psl.in`](einit/src/security.psl.in) template is used to automatically
generate a part of the `security.psl` file using CMake tools. The `security.psl` file contains a
part of a solution security policy description. For more information about the `security.psl` file,
see [Describing a security policy for a KasperskyOS-based solution](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&&customization=KCE&helpid=ssp_descr).

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

### Building and running the example

The example is built using the CMake build system, which is provided in the KasperskyOS Community
Edition SDK. When you develop a KasperskyOS-based solution, use the
[recommended structure of project directories](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=cmake_using_sdk_cmake)
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

[⬆ Back to Top](#table-of-contents)

#### CMake input files

[`./client/CMakeLists.txt`](client/CMakeLists.txt)—CMake commands for building the `Client` program.

[`./einit/CMakeLists.txt`](einit/CMakeLists.txt)—CMake commands for building the `Einit` program
and the solution image.

[`./server/CMakeLists.txt`](server/CMakeLists.txt)—CMake commands for building the `Server` program.

[`./CMakeLists.txt`](CMakeLists.txt)—CMake commands for building the solution.

To generate transport proxies and stubs during image build, use the CMake commands `add_nk_idl()`,
`add_nk_cdl()`, and `add_nk_edl()`.

CMake command `add_nk_idl()` creates CMake target to generate needed files including `*.idl.cpp.h`
for given IDL file `nkppmeta` compiler. It also creates a library containing transport code for the
given interface. To link to this library the `bind_nk_targets()` command is required. Generated
headers contain C++ representation for interface and data types described in IDL file and methods
required for using proxy objects and stubs. Generated headers contain C++ representation for
interface and data types, which are described in IDL file, and the methods which are required for
using proxy objects and stubs.

CMake command `add_nk_cdl()` creates CMake target to generate `*.cdl.cpp.h` for given CDL file with
`nkppmeta` compiler. It also creates a library containing transport code for the given component. To
link to this library the `bind_nk_targets()` command is required. The `*.cdl.cpp.h` file contains
the tree of nested components and endpoints of component in CDL file.

CMake command `add_nk_edl()` creates CMake target to generate `*.edl.cpp.h` for given EDL file with
`nkppmeta` compiler. The command also creates a library containing transport code for the server. To
link to this library the `bind_nk_targets()` command is required. The `*.edl.cpp.h` file contains
the tree of nested components and endpoints of the process class in the EDL file.

## Usage

[Build and run](#building-and-running-the-example) the example. After running the example, the
following actions will be executed:

1. The KasperskyOS kernel runs the `Einit` initialization process.
1. The `Einit` initializes static IPC channels and runs the `Client` and `Server` processes.
1. The `Client` uses endpoints which are provided by the `Server` with IPC.
1. On successful completion of work, the `Client` outputs the `SUCCESS` message to the standard
output. The expected output is contained in the [`./expected_output.txt`](expected_output.txt) file.

[⬆ Back to Top](#table-of-contents)

© 2026 AO Kaspersky Lab
