# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

# The headers are available via cpp-jwt::cpp-jwt target.

include (FetchContent)

# Check for a pre-provided cpp-jwt source directory.
# If set, its source tree is integrated into the build.
if (JWT_SOURCE_DIR)
    add_subdirectory ("${JWT_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/cpp-jwt" EXCLUDE_FROM_ALL)
else ()
    message (STATUS "JWT_SOURCE_DIR is not set, cpp-jwt will be downloaded")
    # Declare and fetch the specific version of the CPP-JWT library.
    FetchContent_Declare (cpp-jwt
                          GIT_REPOSITORY https://github.com/arun11299/cpp-jwt.git
                          GIT_TAG        4b66cf74e5ece16e7f7e8c3d8c0c63d01b4cc9aa)
    # Execute the download and integration of the CPP-JWT library into the build system.
    FetchContent_MakeAvailable(cpp-jwt)
endif ()
