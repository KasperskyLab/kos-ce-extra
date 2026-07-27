# © 2026 AO Kaspersky Lab
# Licensed under the MIT License

cmake_minimum_required (VERSION 3.29)

project (hello)

# Initialize CMake library for the KasperskyOS SDK.
include (platform)
# Enable legacy dynamic linking (statically links libkos, libc, etc. to the loader)
# for compatibility with projects created using earlier versions of the SDK.
initialize_platform (USE_DYNAMIC_LEGACY)

# Set compiler flags.
project_header_default ("STANDARD_GNU_17:YES" "STRICT_WARNINGS:YES")

# Find the vfs package with the VFS component libraries.
find_package (vfs REQUIRED)
include_directories (${vfs_INCLUDE})

# Find the precompiled_vfs package with the VFS component executable files.
find_package (precompiled_vfs REQUIRED)
include_directories (${precompiled_vfs_INCLUDE})

# Find the python package with the Python 3 standard library.
find_package (python REQUIRED COMPONENTS PYTHON3 STDLIB)

set (RESOURCES ${CMAKE_SOURCE_DIR}/resources)

# Configure the Python interpreter arguments and environment for the solution.
# Use ${RESOURCES}/hdd as the root of the solution's file system.
set (PYTHON_EXTRA_ARGS "\
    - /hello.py")
set (PYTHON_EXTRA_ENV  "")

add_subdirectory (einit)
