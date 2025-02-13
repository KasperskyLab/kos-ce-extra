# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

cmake_minimum_required (VERSION 3.25)

project (execution_manager_separated_vfs)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# Initialize CMake library for the KasperskyOS SDK.
include (platform)
initialize_platform ()

# Include the CMake library named install, which contains artifact installation
# functions.
include (platform/install)

# Connect the CMake library named nk for working with the NK compiler
# (nk-gen-c).
include (platform/nk)

# Detect board.
if (CMAKE_SYSTEM_SOC STREQUAL "BCM2711")
    set (BOARD "RPI4_BCM2711")
elseif (CMAKE_SYSTEM_SOC STREQUAL "RK3568")
    set (BOARD "RADXA_ROCK_3A")
else ()
    message (FATAL_ERROR "Unsupported board for SoC '${CMAKE_SYSTEM_SOC}'")
endif ()

project_header_default ("STANDARD_GNU_17:YES" "EXTRA_STRICT_WARNINGS:YES")

# Include the vfs package and its FS_LIB, CLIENT_LIB, and SERVER_LIB components
# for working with the virtual file system.
find_package (vfs REQUIRED COMPONENTS FS_LIB CLIENT_LIB SERVER_LIB)

# Include the vfs package with the VFS component executable files.
find_package (precompiled_vfs REQUIRED)
include_directories (${precompiled_vfs_INCLUDE})

# Include the blob_container package, which provides the capability to load
# dynamic libraries into shared memory.
find_package (blob_container REQUIRED)

# Include the ns package importing components for working with a name server.
find_package (ns REQUIRED)

# Include the execution_manager package for working with the ExecutionManager
# component.
find_package (execution_manager REQUIRED)
include_directories (${execution_manager_INCLUDE})

set (RESOURCES  ${CMAKE_SOURCE_DIR}/resources)
set (ROOTFS_DIR ${CMAKE_BINARY_DIR}/hdd)

# Add the recommended flags against vulnerabilities.
add_compile_options (-Wall -Wextra -Wconversion
                     -fPIE -D_FORTIFY_SOURCE=2 -O2 -fstack-protector-strong
                     -mbranch-protection=standard
                     -Wsign-conversion -Wformat=2 -Wformat-security -Werror=format-security
                     -fsanitize=undefined -fsanitize-undefined-trap-on-error)

add_link_options (LINKER:-z,noexecstack
                  LINKER:-z,separate-code
                  LINKER:-z,now)

add_subdirectory (execution_manager)
add_subdirectory (client)
add_subdirectory (application)
add_subdirectory (einit)
