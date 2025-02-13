# nkppmeta compiler usage example

The KasperskyOS SDK includes the nkppmeta compiler for generating transport code in C++. The nkppmeta
compiler lets you generate transport C++ proxy objects and stubs for use by both a client and a server.

Proxy objects are used by the client to pack the parameters of the called method into an IPC request,
execute the IPC request, and unpack the IPC response. Stubs are used by the server to unpack the
parameters from the IPC request, dispatch the call to the appropriate method implementation, and
pack the IPC response. For more information, please refer to the
[Transport code in C++](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.3&customization=KCE&helpid=cpp_proxy_stubs).

For additional details on KasperskyOS, including its limitations and known issues, please refer to the
[KasperskyOS Community Edition Online Help](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.3&customization=KCE&helpid=community_edition).

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
- [Trademarks](#trademarks)

## Solution overview

### List of programs

* `Client`—Program that interacts with endpoints provided by the server.
* `Server`—Program that contains several nested components and provides endpoints for the
`example.Animal` interface.

To make the client call an interface method provided by the server, the following steps are taken in the
[`./client/src/main.cpp`](client/src/main.cpp) file:

1. The generated client description header `Client.edl.cpp.h` is included.
1. The generated header files of the descriptions of the utilized interfaces `*.idl.cpp.h` are included.
In this example, only `Animal.idl.cpp.h` is included.
1. The `kosipc/make_application.h` and corresponding `kosipc/connect_*.h` headers are included.
In this example, only `kosipc/connect_static_channel.h` is included.
1. The application object is initialized with the `kosipc::MakeApplicationAutodetect()` function.
1. The proxy object for the endpoint to be used is initialized with the `Application::MakeProxy()`
function by selecting one of the `Connect*()` functions as an argument. In this example, the
`ConnectStaticChannel()` function is used because the connection to the endpoint uses the static channel
described in the `init.yaml` file.
1. The corresponding method of the proxy object is called.

To make the server provide endpoints to other processes, the following steps are taken in the
[`./server/src/main.cpp`](server/src/main.cpp) file:

1. The generated server description header `Server.edl.cpp.h` that contains the component structure of the
server including all endpoints is included.
1. The `kosipc/make_application.h` and corresponding `kosipc/serve_*.h` headers are included. In this
example, only `kosipc/serve_static_channel.h` is included.
1. Classes that contain implementations of the interfaces (here it is only `Animal`) that this program
and its components provide as endpoints are created.
1. The application object is initialized with the `kosipc::MakeApplicationAutodetect()` function.
1. The `kosipc::components::Root` structure which describes the component structure and interfaces of
program endpoints is initialized.
1. The fields of the `Root` structure are associated with the objects that implement the corresponding
endpoints. The fields of the `Root` structure repeat the hierarchy in the CDL and EDL files.
1. The incoming request processing cycle is created by calling the `Application::MakeEventLoop()`
function and selecting one of the `Serve*()` functions as an argument. In this example the
`ServeStaticChannel()` function is used because server maintains static channel described in
`init.yaml` file. You need to pass the channel name that you want to maintain and the component
structure (`Root`) that contains all the endpoint implementations into the `ServeStaticChannel()`
function.
1. The incoming IPC message dispatch cycle is run by using the `Run()` method of the `kosipc::EventLoop`
object.

### Initialization description

Static IPC channel: `example.Client` → `example.Server`.

The [`./einit/src/init.yaml.in`](einit/src/init.yaml.in) template is used to automatically generate a
part of the solution initialization description file `init.yaml`. For more information about the
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

1. To install [KasperskyOS Community Edition SDK](https://os.kaspersky.com/development/) and run examples
on QEMU or on a hardware platform, make sure you meet all the
[System requirements](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.3&customization=KCE&helpid=system_requirements),
which are listed in the KasperskyOS Community Edition Developer's Guide.
1. [Install](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.3&customization=KCE&helpid=sdk_install_and_remove)
KasperskyOS Community Edition SDK. You can download the latest version of the KasperskyOS Community
Edition for free from [os.kaspersky.com](https://os.kaspersky.com/development/). The minimum required
version of KasperskyOS Community Edition SDK is 1.3. For more information, see
[System requirements](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.3&customization=KCE&helpid=system_requirements).
1. Copy source files to your project directory.

### Building and running the example

The example is built using the CMake build system, which is provided in the KasperskyOS Community
Edition SDK.

To build the example, run the [`./cross-build.sh`](cross-build.sh) script. There are environment
variables that affect the build of the example:

* `SDK_PREFIX` specifies the path to the installed version of the KasperskyOS Community Edition SDK.
If the `-s` option is not specified when the `cross-build.sh` script has been run, it is necessary to set
the value of the `SDK_PREFIX` environment variable.
* `TARGET` specifies the target platform. (Currently only the `aarch64-kos` platform is supported.)

Syntax for using the `cross-build.sh` script:

`$ ./cross-build.sh [-h] [-l] [-t TARGETS] [-s PATH] [-b PATH]`,

where:

* `-h, --help`

  Help text.
* `-l, --list`

  List of all CMake targets which are available for building.
* `-t, --target TARGETS`

  List of CMake targets that you want to build. Enclose the entire list of targets in quotation marks
and separate the individual targets within the quotation marks with a space character. If not specified,
the default target `sim` will be built.
* `-s, --sdk-path PATH`

  Path to the installed version of the KasperskyOS Community Edition SDK.
The path must be set using either the value of the `SDK_PREFIX` environment variable or the `-s` option.
The value specified in the `-s` option takes precedence over the value of the `SDK_PREFIX` environment variable.
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

Running `cross-build.sh` creates a KasperskyOS-based solution image that includes the example.
The `kos-qemu-image` solution image is located in the `./<build_path>/einit` directory,
where `build_path` is the path to the generated CMake build directory.

The `cross-build.sh` script both builds the example on QEMU and runs it.

#### Hardware

Running `cross-build.sh` creates a KasperskyOS-based solution image that includes the example
and a bootable SD card image for Raspberry Pi 4 B or Radxa ROCK 3A. The `kos-image` solution image is located
in the `./<build_path>/einit` directory. The `hdd.img` bootable SD card image is located
in the `./<build_path>` directory.

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

[⬆ Back to Top](#Table-of-contents)

#### CMake input files

When you develop a KasperskyOS-based solution, use the
[recommended structure of project directories](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.3&customization=KCE&helpid=cmake_using_sdk_cmake)
to simplify usage of CMake scripts.

The `CMakeLists.txt` files use standard CMake syntax and scripts from the `platform`, `nk`, and `image`
SDK libraries.

[`./сlient/CMakeLists.txt`](сlient/CMakeLists.txt)—CMake commands for building the `Сlient` program.

[`./einit/CMakeLists.txt`](einit/CMakeLists.txt)—CMake commands for building the `Einit` program
and the solution image.

[`./server/CMakeLists.txt`](server/CMakeLists.txt)—CMake commands for building the `Server` program.

[`./CMakeLists.txt`](CMakeLists.txt)—CMake commands for building the solution.

To generate transport proxies and stubs during image build, use the CMake commands `add_nk_idl()`,
`add_nk_сdl()`, and `add_nk_edl()`.

CMake command `add_nk_idl()` creates CMake target to generate needed files including `*.idl.cpp.h` for
given IDL file `nkppmeta` compiler. It also creates a library containing transport code for the given
interface. To link to this library the `bind_nk_targets()` command is required. Generated headers
contain C++ representation for interface and data types described in IDL file and methods required for
using proxy objects and stubs.
Generated headers contain C++ representation for interface and data types, which are described in
IDL file, and the methods which are required for using proxy objects and stubs.

CMake command `add_nk_cdl()` creates CMake target to generate `*.cdl.cpp.h` for given CDL file with
`nkppmeta` compiler. It also creates a library containing transport code for the given component. To
link to this library the `bind_nk_targets()` command is required. The `*.cdl.cpp.h` file contains the
tree of nested components and endpoints of component in CDL file.

CMake command `add_nk_edl()` creates CMake target to generate `*.edl.cpp.h` for given EDL file with
`nkppmeta` compiler. The command also creates a library containing transport code for the server. To
link to this library the `bind_nk_targets()` command is required. The `*.edl.cpp.h` file contains the
tree of nested components and endpoints of the process class in the EDL file.

## Usage

[Build and run](#building-and-running-the-example) the example. After running the example, the following
actions will be executed:

1. The KasperskyOS kernel runs the `Einit` initialization process.
1. The `Einit` initializes static IPC channels and runs the `Client` and `Server` processes.
1. The `Client` uses endpoints which are provided by the `Server` with IPC.
1. On successful completion of work, the `Client` outputs the `SUCCESS` message to the standard output.
The expected output is contained in the [`./expected_output.txt`](expected_output.txt) file.

# Trademarks

Registered trademarks and endpoint marks are the property of their respective owners.

Raspberry Pi is a trademark of the Raspberry Pi Foundation.

[⬆ Back to Top](#Table-of-contents)

© 2025 AO Kaspersky Lab
