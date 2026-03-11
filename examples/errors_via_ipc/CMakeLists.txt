# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

cmake_minimum_required (VERSION 3.29)

project (errors_via_ipc_example)
# Initialize the CMake library for the KasperskyOS SDK.
include (platform)
# Statically link executable files.
initialize_platform (FORCE_STATIC)

# Set compiler flags.
project_header_default ("STANDARD_GNU_17:YES" "EXTRA_STRICT_WARNINGS:YES")

# Connect the CMake library named nk for working with the NK compiler (nk-gen-c).
include (platform/nk)

set (fmt_USE_STATIC ON)

# Attach packages with LogRR CPP and CLOG.
find_package (logrr_cpp REQUIRED)
find_package (logrr_clog REQUIRED)

# Attach the vfs package with the VFS component executable files.
find_package (precompiled_vfs REQUIRED)
include_directories (${precompiled_vfs_INCLUDE})

set (RESOURCES ${CMAKE_SOURCE_DIR}/resources)
set (NK_RESOURCES ${RESOURCES}/xdl)
nk_include_directories (${NK_RESOURCES})

# Set recommended compiler flags against vulnerabilities.
add_compile_options (-fPIE -Wall -Wextra -Wconversion -Wsign-conversion
                     -Wformat=2 -Wformat-security -Werror=format-security)

add_subdirectory (server_c)
add_subdirectory (client_c)
add_subdirectory (server_cpp)
add_subdirectory (client_cpp)
add_subdirectory (einit)
