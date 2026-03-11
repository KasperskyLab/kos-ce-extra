# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

cmake_minimum_required (VERSION 3.29)

project (wpa_client)

# Initialize CMake library for KasperskyOS SDK.
include (platform)
# Statically link executable files.
initialize_platform (FORCE_STATIC)

if (PLATFORM_SUPPORTS_DYNAMIC_LINKING)
    message (FATAL_ERROR "The solution only supports static libraries")
endif ()

# Include the CMake library named nk for working with the NK compiler (nk-gen-c).
include (platform/nk)

# Include the CMake library named install, which contains artifact installation functions.
include (platform/install)

# Set compilation flags.
project_header_default ("STANDARD_GNU_17:YES" "EXTRA_STRICT_WARNINGS:YES")

# Add the vfs package with the VFS component libraries.
find_package (vfs REQUIRED)
include_directories (${vfs_INCLUDE})

# Add the rump package with the Dhcpcd program implementation.
find_package (rump REQUIRED COMPONENTS DHCPCD_ENTITY)
include_directories (${rump_INCLUDE})

# Add the vfs package with the VFS component executable files.
find_package (precompiled_vfs REQUIRED)
include_directories (${precompiled_vfs_INCLUDE})

# Add package with wpa_supplicant program implementations.
find_package (wpa REQUIRED)

set(Boost_NO_WARN_NEW_VERSIONS 1)
find_package (Boost COMPONENTS program_options REQUIRED)

find_package (dnet_executables REQUIRED)
find_package (dnet_brcm_fmac REQUIRED)

set (RESOURCES ${CMAKE_SOURCE_DIR}/resources)
set (NK_RESOURCES ${RESOURCES}/xdl)
# Add custom include directories for the NK compiler.
nk_include_directories (${NK_RESOURCES})

# Add recommended flags against vulnerabilities.
add_compile_options (-fPIE -Wall -Wextra -Wconversion -Wsign-conversion
                     -Wformat=2 -Wformat-security -Werror=format-security)

add_subdirectory (client)
add_subdirectory (einit)
