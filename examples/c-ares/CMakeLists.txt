# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

cmake_minimum_required (VERSION 3.29)
project (example)

# Initialize CMake library for the KasperskyOS SDK.
include (platform)
initialize_platform (USE_DYNAMIC_LEGACY)

# Set compiler flags.
project_header_default ("STANDARD_GNU_17:YES" "STRICT_WARNINGS:YES")

# Include the CMake library named nk for working with the NK compiler (nk-gen-c).
include (platform/nk)

find_package (Threads REQUIRED)

# Add a package for working with the virtual file system.
find_package (vfs REQUIRED)
include_directories (${vfs_INCLUDE})

# Add a package with the VFS program implementations.
find_package (precompiled_vfs REQUIRED)
include_directories (${precompiled_vfs_INCLUDE})

# Add a package with the Dhcpcd program implementation.
find_package (rump REQUIRED COMPONENTS DHCPCD_ENTITY)
include_directories (${rump_INCLUDE})

# Add a package with LogRR CPP.
find_package (logrr_cpp REQUIRED)

# Include the directory with the generated config header files.
include_directories (${CMAKE_CURRENT_BINARY_DIR})

# Include the directory with header files.
include_directories (${CMAKE_CURRENT_LIST_DIR})

# Directories with resource files.
set (RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources)
set (RESOURCES_HDD ${RESOURCES}/hdd)
set (RESOURCES_XDL ${RESOURCES}/xdl/example)

# Create a CMake target for generating a *.idl.cpp.h header file for a defined IDL file using the
# nkppmeta compiler
add_nk_idl (RESOLVER_IDL "${RESOURCES_XDL}/Resolver.idl" NK_MODULE example LANG "CXX")

add_nk_idl (RESOLVE_CALLBACK_IDL "${RESOURCES_XDL}/ResolveCallback.idl" NK_MODULE example
            LANG "CXX")

set (RESOLVE_IDLS RESOLVER_IDL RESOLVE_CALLBACK_IDL)

# Set recommended flags against vulnerabilities.
add_compile_options (-fPIE -Wall -Wextra -Wconversion -Wsign-conversion
                     -Wformat=2 -Wformat-security -Werror=format-security)

add_subdirectory (resolver)
add_subdirectory (client)
add_subdirectory (einit)
