# © 2026 AO Kaspersky Lab
# Licensed under the MIT License

cmake_minimum_required (VERSION 3.29)
project (jwt_example)

# Initialize CMake library for the KasperskyOS SDK.
include (platform)
initialize_platform (USE_DYNAMIC_LEGACY)

# Set compiler flags.
project_header_default ("STANDARD_GNU_17:YES" "EXTRA_STRICT_WARNINGS:YES")

# Include the CMake library named nk
# for working with the NK compiler (nk-gen-c).
include (platform/nk)

# Integrate the OpenSSL library into the current build.
include (cmake/openssl.cmake)

# Integrate the CPP-JWT library into the current build.
include (cmake/cpp-jwt.cmake)

# Attach the vfs package with the VFS component libraries.
find_package (vfs REQUIRED)
include_directories (${vfs_INCLUDE})

# Attach a package with the Dhcpcd program implementation.
find_package (rump REQUIRED COMPONENTS DHCPCD_ENTITY)
include_directories (${rump_INCLUDE})

# Attach the vfs package with the VFS component executable files.
find_package (precompiled_vfs REQUIRED)
include_directories (${precompiled_vfs_INCLUDE})

# Attach the logrr_cpp package with the LogRR component.
find_package (logrr_cpp REQUIRED)

find_package (Boost REQUIRED COMPONENTS program_options)
include_directories (${Boost_INCLUDE_DIRS})

find_package (OpenSSL REQUIRED)
find_package (Threads REQUIRED)

# Attach the directory with the generated config header files.
include_directories (${CMAKE_CURRENT_BINARY_DIR})

# Attach the directory with header files.
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/resources/include)

# Include the directory with header files.
set (RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources)
set (NK_RESOURCES ${RESOURCES}/xdl/)
nk_include_directories (${NK_RESOURCES})

# Add recommended flags against vulnerabilities.
add_compile_options (-fPIE -Wall -Wextra -Wconversion -Wsign-conversion
                     -Wformat=2 -Wformat-security -Werror=format-security)

# Attach the directories with programs.
add_subdirectory (token_service)
add_subdirectory (web_server)

# Attach the einit directory containing the KasperskyOS image
# to the project build.
add_subdirectory (einit)
