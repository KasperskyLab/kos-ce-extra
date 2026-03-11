# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

cmake_minimum_required (VERSION 3.29)
project (mbim_sms)

# Initialize CMake library for KasperskyOS SDK.
include (platform)
# Statically link executable files.
initialize_platform (FORCE_STATIC)

# Include the CMake library named nk for working with the NK compiler (nk-gen-c).
include (platform/nk)

# Set compiler flags.
project_header_default ("STANDARD_GNU_17:YES" "EXTRA_STRICT_WARNINGS:YES")

# Include the vfs package with the VFS component libraries.
find_package (vfs REQUIRED)
include_directories (${vfs_INCLUDE})

# Include the precompiled_vfs package with the VFS component executable files.
find_package (precompiled_vfs REQUIRED)
include_directories (${precompiled_vfs_INCLUDE})

# Include the dnet_executables package with DNet driver executables which includes MBIM driver.
find_package (dnet_executables REQUIRED)

# Set recommended compiler flags against vulnerabilities.
add_compile_options (-fPIE -Wall -Wextra -Wconversion -Wsign-conversion
                     -Wformat=2 -Wformat-security -Werror=format-security)

add_subdirectory (client)
add_subdirectory (einit)
