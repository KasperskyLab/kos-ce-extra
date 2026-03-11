# © 2026 AO Kaspersky Lab
# Licensed under the MIT License

# The headers are available via openssl_headers target.

include (FetchContent)

if (NOT OPENSSL_INCLUDE_DIR)
    message (STATUS "OPENSSL_INCLUDE_DIR is not set, OpenSSL headers will be downloaded")
    # Declare and fetch the specific version of the OpenSSL library.
    FetchContent_Declare (openssl
                          GIT_REPOSITORY https://github.com/openssl/openssl.git
                          GIT_TAG        OpenSSL_1_1_1t
                          GIT_SUBMODULES ""
                          GIT_SHALLOW    ON)

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
    add_custom_target (openssl_headers
                       DEPENDS           ${openssl_SOURCE_DIR}/Makefile
                       COMMAND           make build_generated
                       WORKING_DIRECTORY ${openssl_SOURCE_DIR})
else ()
    add_custom_target (openssl_headers)
endif ()
