# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

cmake_minimum_required (VERSION 3.29)

project (sequence_example)

# Initialize the CMake library for the KasperskyOS SDK.
include (platform)
# Statically link executable files.
initialize_platform (FORCE_STATIC)

# Connect the CMake library named nk for working with the NK compiler (nk-gen-c).
include (platform/nk)

# Set the linker and compiler flags.
project_header_default ("STANDARD_GNU_17:YES" "EXTRA_STRICT_WARNINGS:YES")

set (RESOURCES ${CMAKE_SOURCE_DIR}/resources)
set (NK_RESOURCES ${RESOURCES}/xdl)
nk_include_directories (${NK_RESOURCES})

# Set recommended compiler flags against vulnerabilities.
add_compile_options (-fPIE -Wall -Wextra -Wconversion -Wsign-conversion
                     -Wformat=2 -Wformat-security -Werror=format-security)

include_directories (${RESOURCES}/include)

add_subdirectory (reader)
add_subdirectory (writer)
add_subdirectory (einit)
