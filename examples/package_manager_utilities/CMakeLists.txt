# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

cmake_minimum_required (VERSION 3.29)

project (package_manager_utilities)

# Initialize the CMake library for the KasperskyOS SDK.
include (platform)
initialize_platform (USE_DYNAMIC_LEGACY)

# Set compiler flags.
project_header_default ("STANDARD_GNU_17:YES" "EXTRA_STRICT_WARNINGS:YES")

# Include the CMake library named install, which contains artifact installation functions.
include (platform/install)

# Include the CMake library named nk for working with the NK compiler (nk-gen-c).
include (platform/nk)

# Include the vfs package and its FS_LIB, CLIENT_LIB, and SERVER_LIB components for working with the
# VFS component.
find_package (vfs REQUIRED COMPONENTS FS_LIB CLIENT_LIB SERVER_LIB)

# Include the precompiled_vfs package with the VFS component executable files.
find_package (precompiled_vfs REQUIRED)
include_directories (${precompiled_vfs_INCLUDE})

# Include the package_manager package for working with the PackageManager component.
find_package (package_manager REQUIRED)

# Include the execution_manager package for working with the ExecutionManager component.
find_package (execution_manager REQUIRED)
include_directories (${execution_manager_INCLUDE})

set (RESOURCES ${CMAKE_SOURCE_DIR}/resources)
set (NK_RESOURCES ${RESOURCES}/xdl)
nk_include_directories (${NK_RESOURCES})

set (ROOTFS_DIR      ${CMAKE_BINARY_DIR}/hdd)
set (PACKAGE_ID      "kl.pmexample")
set (PACKAGE_VERSION "1.0.0")
set (APP_INSTALL_DIR bin)
set (PKG_DB_PATH     "/repository.sqlite")

# Set recommended flags against vulnerabilities.
add_compile_options (-fPIE -Wall -Wextra -Wconversion -Wsign-conversion
                     -Wformat=2 -Wformat-security -Werror=format-security)

add_subdirectory (application)
add_subdirectory (launcher)
add_subdirectory (package_manager)
add_subdirectory (execution_manager)
add_subdirectory (kpa_package)
add_subdirectory (einit)
