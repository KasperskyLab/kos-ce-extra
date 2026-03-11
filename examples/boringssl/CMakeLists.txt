# © 2026 AO Kaspersky Lab
# Licensed under the MIT License

cmake_minimum_required (VERSION 3.29)

project (hasher)

# Initialize CMake library for the KasperskyOS SDK.
include (platform)
# Statically link executable files.
initialize_platform (FORCE_STATIC)

# Integrate the KasperskyOS-adapted BoringSSL library into the current build.
include (cmake/BoringSSL.cmake)

# Set the linker and compiler flags.
project_header_default ("STANDARD_GNU_17:YES" "EXTRA_STRICT_WARNINGS:NO")

# Include the CMake library named nk for working with the NK compiler (nk-gen-c).
include (platform/nk)

add_subdirectory (hasher)
add_subdirectory (einit)
