# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

cmake_minimum_required (VERSION 3.29)

project (hello_world)

# Set the variable SSL_CERTIFICATE_DIR to the directory path where the SSL certificates are located.
set (RESOURCES "${CMAKE_SOURCE_DIR}/resources")
set (SSL_CERTIFICATE_DIR "${RESOURCES}/test_creds")

if (KOS)
    # Include the CMake library named platform containing common CMake scripts for platform
    # definition.
    include (platform)
    initialize_platform (USE_DYNAMIC_LEGACY)

    # Set compiler flags.
    project_header_default ("STANDARD_GNU_17:YES" "EXTRA_STRICT_WARNINGS:YES")

    # Include the CMake library named nk for working with the NK compiler (nk-gen-c).
    include (platform/nk)

    # Include the vfs package with the VFS component libraries.
    find_package (vfs REQUIRED)
    include_directories (${vfs_INCLUDE})

    # Search for the rump package and load its DHCPCD_CLIENT_LIB component.
    find_package (rump REQUIRED COMPONENTS DHCPCD_ENTITY)
    include_directories (${rump_INCLUDE})

    # Include the precompiled_vfs package with VFS executable files.
    find_package (precompiled_vfs REQUIRED)
    include_directories (${precompiled_vfs_INCLUDE})

    # Include the NTPD package by locating the ntpd executable in system paths, failing the build if
    # not found.
    find_program (NTPD
        NAMES "ntpd" REQUIRED
        NO_CMAKE_PATH
        NO_CMAKE_ENVIRONMENT_PATH
        NO_SYSTEM_ENVIRONMENT_PATH)

    find_package (Threads REQUIRED)
endif ()

add_subdirectory (libraries)

if (KOS)
    message (STATUS "Building ${BUILD_TARGET}")
    if (BUILD_TARGET STREQUAL "client")
        add_subdirectory (client)
    elseif (BUILD_TARGET STREQUAL "server")
        add_subdirectory (server)
    else ()
        message (FATAL_ERROR "Unknown target: ${BUILD_TARGET}")
    endif()
    add_subdirectory (einit)
else ()
    add_subdirectory(server)
    add_subdirectory(client)
endif ()
