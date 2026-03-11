# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

cmake_minimum_required (VERSION 3.29)
project (amqp)

# Initialize CMake library for the KasperskyOS SDK.
include (platform)
initialize_platform (USE_DYNAMIC_LEGACY)

# Set compiler flags.
project_header_default ("STANDARD_GNU_17:YES" "EXTRA_STRICT_WARNINGS:YES")

# Include the CMake library named nk for working with the NK compiler (nk-gen-c).
include (platform/nk)

# Include the logrr_cpp package with the logrr_cpp library.
find_package (logrr_cpp REQUIRED)

# Include the vfs package with the VFS component libraries.
find_package(vfs REQUIRED)
include_directories(${vfs_INCLUDE})

# Include the rump package with the Dhcpcd program implementation.
find_package(rump REQUIRED COMPONENTS DHCPCD_ENTITY)
include_directories(${rump_INCLUDE})

# Include the precompiled_vfs package with the VFS component executable files.
find_package(precompiled_vfs REQUIRED)
include_directories(${precompiled_vfs_INCLUDE})

set (RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources)
set (NK_RESOURCES ${RESOURCES}/xdl)
# Add custom include directories for the NK compiler.
nk_include_directories (${NK_RESOURCES})

set (COMMON ${CMAKE_CURRENT_SOURCE_DIR}/../common)
set (UTILS ${COMMON}/utils)

# Look for a pre-installed RabbitMQ-C library (rabbitmq-c package).
find_package (rabbitmq-c QUIET)
# If not found in the system, use embedded version via FetchContent.
if (NOT rabbitmq-c_FOUND)
    # Include the FetchContent module that downloads and builds RabbitMQ-C library with its
    # dependencies, applying KasperskyOS-specific adaptations.
    include (${COMMON}/lib/CMakeLists.txt)
endif ()

add_subdirectory(publisher)

# Include the einit directory containing the KasperskyOS image to the project build.
add_subdirectory(einit)
