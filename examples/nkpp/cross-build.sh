#!/bin/bash

# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

SCRIPT_DIR="$(dirname "$(realpath "${0}")")"
SCRIPT_NAME="$(basename "$(realpath "${0}")")"
BUILD=${SCRIPT_DIR}/build

if [ -z "${SDK_PREFIX}" ]; then
    export SDK_PREFIX=""
fi

export LANG=C
# Target architecture.
# Change this value to the desired architecture if required.
export TARGET="aarch64-kos"
export PKG_CONFIG=""
export INSTALL_PREFIX="$BUILD/../install"

export BUILD_WITH_CLANG=
export BUILD_WITH_GCC=

TOOLCHAIN_SUFFIX=""

if [ "$BUILD_WITH_CLANG" == "y" ];then
    TOOLCHAIN_SUFFIX="-clang"
fi

if [ "$BUILD_WITH_GCC" == "y" ];then
    TOOLCHAIN_SUFFIX="-gcc"
fi

CMAKE_TARGETS="sim"

function PrintHelp () {
    cat<<HELP
${SCRIPT_NAME} [-h | --help]
${SCRIPT_NAME} [-l | --list]
${SCRIPT_NAME} [-b | --build <path>] [-s | --sdk-path <path to SDK>] [-t | --target <targets>]
Script for building and running the nkpp example for KasperskyOS.
This script builds a KasperskyOS-based solution image for each target in the CMAKE_TARGETS variable.
The image will be run on QEMU if the target is sim.


-h, --help                Help text.
-l, --list                List of all available CMake targets.
-t, --target <targets>    List of CMake targets that you want to build.
                          Enclose the entire list of targets in quotation marks and separate the
                          individual targets within the quotation marks with a space character.
                          If not specified, the default target "${CMAKE_TARGETS}" will be built.
-s, --sdk-path <path>     Path to the installed version of the KasperskyOS Community Edition SDK.
                          The path must be set using either the value of the SDK_PREFIX environment
                          variable or the -s option. The value specified in the -s option takes
                          precedence over the value of the SDK_PREFIX environment variable.
                          If not specified, the default path "${SDK_PREFIX}" will be used.
-b, --build <path>        Path to the generated CMake build directory.
                          If not specified, the default path "${BUILD}" will be used.
HELP
}

function ParseArguments () {
    while [ -n "${1}" ]; do
        case "${1}" in
        -h | --help)
            PrintHelp
            exit 0;;
        -l | --list)
            _CMAKE_TARGETS="help"
            break;;
        -t | --target)
            _CMAKE_TARGETS="${2}"
            shift;;
        -s | --sdk-path)
            _SDK_PREFIX=$2
            shift;;
        -b | --build)
            _BUILD=$2
            shift;;
        *) echo "Unknown option was passed: '${1}'."
            exit 1;;
        esac
        shift
    done

    if [ "${_SDK_PREFIX}" ]; then
        export SDK_PREFIX=$_SDK_PREFIX
    fi
    if [ "${_CMAKE_TARGETS}" ]; then
        CMAKE_TARGETS=$_CMAKE_TARGETS
    fi
    if [ "${_BUILD}" ]; then
        BUILD=$_BUILD
    fi
}

ParseArguments "$@"

if [ -z "${SDK_PREFIX}" ]; then
    echo "Can't get the path to the installed KasperskyOS SDK."
    PrintHelp
    exit 1
fi

export PATH="$SDK_PREFIX/toolchain/bin:$PATH"

"$SDK_PREFIX/toolchain/bin/cmake" -G "Unix Makefiles" -B "$BUILD" \
      --no-warn-unused-cli \
      -D CMAKE_BUILD_TYPE:STRING=Debug \
      -D CMAKE_INSTALL_PREFIX:STRING="$INSTALL_PREFIX" \
      -D CMAKE_FIND_ROOT_PATH="$([[ -f "$SCRIPT_DIR/additional_cmake_find_root.txt" ]] && cat "$SCRIPT_DIR/additional_cmake_find_root.txt")$PREFIX_DIR/sysroot-$TARGET" \
      -D CMAKE_TOOLCHAIN_FILE="$SDK_PREFIX/toolchain/share/toolchain-$TARGET$TOOLCHAIN_SUFFIX.cmake" \
      "$SCRIPT_DIR/" && "$SDK_PREFIX/toolchain/bin/cmake" --build "$BUILD" --target $CMAKE_TARGETS
