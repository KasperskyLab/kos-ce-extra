# © 2026 AO Kaspersky Lab
# Licensed under the MIT License

# This script downloads and configures RabbitMQ-C and OpenSSL dependencies using CMake's
# FetchContent.

cmake_minimum_required (VERSION 3.29)

project (rabbitmq-c-kos)

include (FetchContent)

# Enable verbose output for download progress.
set (FETCHCONTENT_QUIET OFF)

# Configure RabbitMQ-C repository and version.
set (RABBITMQ_GIT_REPO https://github.com/alanxz/rabbitmq-c.git)
set (RABBITMQ_GIT_TAG  v0.15.0)

# Declare and fetch the specific version of the OpenSSL library.
FetchContent_Declare (openssl
                      GIT_REPOSITORY https://github.com/openssl/openssl.git
                      GIT_TAG        OpenSSL_1_1_1t
                      GIT_SUBMODULES ""
                      GIT_SHALLOW    ON
                      GIT_PROGRESS   ON)

# Check if OpenSSL is already populated.
FetchContent_GetProperties (openssl)
if (NOT openssl_POPULATED)
    FetchContent_Populate (openssl)
endif ()

# Set the OpenSSL include directory for compiler.
set (OPENSSL_INCLUDE_DIR "${openssl_SOURCE_DIR}/include/")
# Configure the OpenSSL build system.
add_custom_command (OUTPUT            ${openssl_SOURCE_DIR}/Makefile
                    COMMAND           ./config
                    WORKING_DIRECTORY ${openssl_SOURCE_DIR})
# Create a custom target to build OpenSSL.
add_custom_target (openssl
                   DEPENDS           ${openssl_SOURCE_DIR}/Makefile
                   COMMAND           make build_generated
                   WORKING_DIRECTORY ${openssl_SOURCE_DIR})

# Check for a pre-provided RabbitMQ-C source directory.
# If set, its source tree is integrated into the build.
if (RABBITMQ_SOURCE_DIR)
    # Use pre-downloaded RabbitMQ-C sources.
    FetchContent_Declare (rabbitmq
                          SOURCE_DIR ${RABBITMQ_SOURCE_DIR})
else ()
    # Declare and fetch the specific version of RabbitMQ-C.
    message (STATUS "RABBITMQ_SOURCE_DIR is not set, the source code will be downloaded")
    FetchContent_Declare (rabbitmq
                          GIT_REPOSITORY ${RABBITMQ_GIT_REPO}
                          GIT_TAG        ${RABBITMQ_GIT_TAG}
                          GIT_SHALLOW    ON
                          GIT_PROGRESS   ON)
endif ()

# Execute the download and integration of RabbitMQ-C into the build system.
FetchContent_MakeAvailable (rabbitmq)

# Suppress specific warnings in RabbitMQ-C targets.
if (TARGET rabbitmq)
    # Configure the shared library target.
    target_compile_options (rabbitmq PRIVATE "-Wno-error")
    add_dependencies (rabbitmq openssl)
endif ()
if (TARGET rabbitmq-static)
    # Configure the static library target.
    target_compile_options (rabbitmq-static PRIVATE "-Wno-error")
    add_dependencies (rabbitmq-static openssl)
endif ()
