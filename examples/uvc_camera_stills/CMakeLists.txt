# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

cmake_minimum_required (VERSION 3.29)

project (uvc_camera_stills)

# Initialize CMake library for KasperskyOS SDK.
include (platform)
# Statically link executable files.
initialize_platform (FORCE_STATIC)

# Set the linker and compiler flags.
project_header_default ("STANDARD_GNU_17:YES" "EXTRA_STRICT_WARNINGS:YES")

# Include the CMake library named nk for working with the NK compiler (nk-gen-c).
include (platform/nk)

find_package (camera REQUIRED)
include_directories (${camera_INCLUDE})

find_package (vfs REQUIRED)
include_directories (${vfs_INCLUDE})

find_package (precompiled_vfs REQUIRED)
include_directories (${precompiled_vfs_INCLUDE})

# Set recommended flags against vulnerabilities.
add_compile_options (-fPIE -Wall -Wextra -Wconversion -Wsign-conversion
                     -Wformat=2 -Wformat-security -Werror=format-security)

add_subdirectory (client)
add_subdirectory (einit)
