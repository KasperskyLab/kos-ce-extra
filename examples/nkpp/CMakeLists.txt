# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

cmake_minimum_required (VERSION 3.25)

project (nkpp_cpp_stub)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# Initialize the CMake library for the KasperskyOS SDK.
include (platform)
# Statically link executable files.
initialize_platform (FORCE_STATIC)

# Include the CMake library named nk for working with the NK compiler (nk-gen-c).
include (platform/nk)

# Detect board.
if (CMAKE_SYSTEM_SOC STREQUAL "BCM2711")
    set (BOARD "RPI4_BCM2711")
elseif (CMAKE_SYSTEM_SOC STREQUAL "RK3568")
    set (BOARD "RADXA_ROCK_3A")
else ()
    message (FATAL_ERROR "Unsupported board for SoC '${CMAKE_SYSTEM_SOC}'")
endif ()

project_header_default ("STANDARD_GNU_17:YES" "EXTRA_STRICT_WARNINGS:YES")

set (RESOURCES ${CMAKE_SOURCE_DIR}/resources)

# Add the recommended compiler flags against vulnerabilities.
add_compile_options (-Wall -Wextra -Wconversion
                     -fPIE -D_FORTIFY_SOURCE=2 -O2 -fstack-protector-strong
                     -mbranch-protection=standard
                     -Wsign-conversion -Wformat=2 -Wformat-security -Werror=format-security
                     -fsanitize=undefined -fsanitize-undefined-trap-on-error)

add_link_options (LINKER:-z,noexecstack
                  LINKER:-z,separate-code
                  LINKER:-z,now)

# Include the directory with the generated config header files.
include_directories (${CMAKE_CURRENT_BINARY_DIR})

# Include the example directory.
include_directories (${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory (server)
add_subdirectory (client)
add_subdirectory (einit)
