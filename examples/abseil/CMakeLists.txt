# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

cmake_minimum_required (VERSION 3.29)

project (example)

# Initialize CMake library for the KasperskyOS SDK.
include (platform)
initialize_platform (USE_DYNAMIC_LEGACY)

# Set the linker and compiler flags.
project_header_default ("STANDARD_GNU_17:YES" "EXTRA_STRICT_WARNINGS:YES")

# Include the CMake library named nk for working with the NK compiler (nk-gen-c).
include (platform/nk)

# Include the CMake library named options, which contains platform definitions.
include (platform/options)

if (PLATFORM_SUPPORTS_DYNAMIC_LINKING)
    find_package (precompiled_vfs REQUIRED)
endif()

# Ensure the KasperskyOS-adapted Abseil C++ library is pre-built and installed at a known location
# before the absl package search.
find_package (absl REQUIRED)

# Add recommended flags against vulnerabilities.
add_compile_options (-fPIE -Wall -Wextra -Wconversion -Wsign-conversion
                     -Wformat=2 -Wformat-security -Werror=format-security)

add_subdirectory (hello)
add_subdirectory (einit)
