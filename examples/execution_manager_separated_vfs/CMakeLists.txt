# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

cmake_minimum_required (VERSION 3.29)

project (execution_manager_separated_vfs)

# Initialize CMake library for the KasperskyOS SDK.
include (platform)
initialize_platform (USE_DYNAMIC_LEGACY)

# Set the linker and compiler flags.
project_header_default ("STANDARD_GNU_17:YES" "EXTRA_STRICT_WARNINGS:YES")

# Include the CMake library named install, which contains artifact installation
# functions.
include (platform/install)

# Connect the CMake library named nk for working with the NK compiler
# (nk-gen-c).
include (platform/nk)

# Include the vfs package and its FS_LIB, CLIENT_LIB, and SERVER_LIB components
# for working with the virtual file system.
find_package (vfs REQUIRED COMPONENTS FS_LIB CLIENT_LIB SERVER_LIB)

# Include the vfs package with the VFS component executable files.
find_package (precompiled_vfs REQUIRED)
include_directories (${precompiled_vfs_INCLUDE})

# Include the ns package importing components for working with a name server.
find_package (ns REQUIRED)

# Include the execution_manager package for working with the ExecutionManager
# component.
find_package (execution_manager REQUIRED)
include_directories (${execution_manager_INCLUDE})

set (RESOURCES  ${CMAKE_SOURCE_DIR}/resources)
set (NK_RESOURCES ${RESOURCES}/xdl)
nk_include_directories (${NK_RESOURCES})

set (ROOTFS_DIR ${CMAKE_BINARY_DIR}/hdd)

# Add the recommended flags against vulnerabilities.
add_compile_options (-fPIE -Wall -Wextra -Wconversion -Wsign-conversion
                     -Wformat=2 -Wformat-security -Werror=format-security)

add_subdirectory (execution_manager)
add_subdirectory (client)
add_subdirectory (application)
add_subdirectory (einit)
