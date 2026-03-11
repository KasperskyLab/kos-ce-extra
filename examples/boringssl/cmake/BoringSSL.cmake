# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

include (FetchContent)

# Check for a pre-provided BoringSSL source directory.
# If set, its source tree is integrated into the build.
if (BORINGSSL_SOURCE_DIR)
    add_subdirectory ("${BORINGSSL_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/boringssl" EXCLUDE_FROM_ALL)
else ()
    # Attempt to find a pre-installed BoringSSL package on the system.
    find_package (BoringSSL QUIET)
     # If not found locally, download the KasperskyOS-adapted version from the repository.
    if (NOT BoringSSL_FOUND)
        message (STATUS "BORINGSSL_SOURCE_DIR is not set and package is not found, BoringSSL will be downloaded")
        # Declare and fetch the specific adapted version of the BoringSSL library.
        FetchContent_Declare (BoringSSL-kos
                              GIT_REPOSITORY https://github.com/KasperskyLab/boringssl-kos.git
                              GIT_TAG        kosce_v1.4.0
        )
        # Execute the download and integration of the library into the build system.
        FetchContent_MakeAvailable (BoringSSL-kos)
    endif ()
endif ()
