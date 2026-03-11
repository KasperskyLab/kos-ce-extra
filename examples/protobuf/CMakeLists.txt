# © 2026 AO Kaspersky Lab
# Licensed under the MIT License

cmake_minimum_required (VERSION 3.29)

project (protobuf_example)

# Initialize CMake library for the KasperskyOS SDK.
include (platform)
initialize_platform (USE_DYNAMIC_LEGACY)

# Set compiler flags.
project_header_default ("STANDARD_GNU_17:YES" "STRICT_WARNINGS:YES")

# Include the CMake library named nk for working with the NK compiler (nk-gen-c).
include (platform/nk)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include the protobuf package with protobuf runtime library.
find_package (protobuf REQUIRED)

# Find the protobuf compiler (protoc) for the host system.
# This is required for cross-compilation to generate code from .proto files.
find_program (protoc NAMES protoc REQUIRED)

# Include the vfs package with the VFS component libraries.
find_package (vfs REQUIRED)
include_directories (${vfs_INCLUDE})

# Include the precompiled_vfs package with the VFS component executable files.
find_package (precompiled_vfs REQUIRED)
include_directories (${precompiled_vfs_INCLUDE})

# Include the logrr_cpp package with the logrr_cpp library, which provides the API for sending
# messages to the log.
find_package (logrr_cpp REQUIRED)

# Include the directory with the generated config header files.
include_directories (${CMAKE_CURRENT_BINARY_DIR})

# Include the directory with header files.
set (RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources)
include_directories (${RESOURCES}/include)
set (NK_RESOURCES ${RESOURCES}/xdl/)
nk_include_directories (${NK_RESOURCES})

# Add target to generate protobuf files.
set (PROTO_SOURCE_DIR "${RESOURCES}/proto")
set (PROTO_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")

add_library (addressbook OBJECT "${PROTO_SOURCE_DIR}/addressbook.proto")
target_link_libraries (addressbook PUBLIC protobuf::libprotobuf)
target_include_directories (addressbook PUBLIC "$<BUILD_INTERFACE:${PROTO_BINARY_DIR}>")

protobuf_generate (TARGET         addressbook
                   PROTOC_EXE     "${protoc}"
                   IMPORT_DIRS    "${PROTO_SOURCE_DIR}"
                   PROTOC_OUT_DIR "${PROTO_BINARY_DIR}")

add_subdirectory (consumer)
add_subdirectory (producer)
add_subdirectory (einit)
