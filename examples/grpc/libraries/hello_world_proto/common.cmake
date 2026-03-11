# Copyright 2018 gRPC authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# © 2026 AO Kaspersky Lab
# Licensed under the MIT License
#
# CMake build file for the C++ route_guide example.
# This configuration assumes that Protocol Buffers (protobuf) and gRPC are already installed on the
# system via CMake.
# See cmake_externalproject/CMakeLists.txt for an all-in-one CMake build that automatically builds
# all dependencies before building route_guide.

cmake_minimum_required (VERSION 3.29)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED True)

find_package (Threads REQUIRED)

if (GRPC_AS_SUBMODULE)
    # One way to build a project that uses gRPC is to include the entire gRPC project tree via
    # "add_subdirectory". This approach is simple, but there are some potential disadvantages:
    # * It directly includes gRPC's CMakeLists.txt into your build script, which can cause gRPC's
    #   internal settings to interfere with your own build.
    # * Depending on your system, contents of submodules in gRPC's third_party/* might need to be
    #   available (and there might be additional prerequisites). Consider using the gRPC_*_PROVIDER
    #   options to fine-tune the expected behavior.
    #
    # A more robust approach for adding a dependency on gRPC is to use CMake's ExternalProject_Add
    # (see cmake_externalproject/CMakeLists.txt).

    # Include gRPC's CMake build (normally, the gRPC source code would reside in a git submodule
    # named "third_party/grpc", but this example resides in the same repository as gRPC sources, so
    # we reference it a few directories up).
    add_subdirectory (../../.. ${CMAKE_CURRENT_BINARY_DIR}/grpc EXCLUDE_FROM_ALL)
    message (STATUS "Using gRPC via add_subdirectory.")

    # After using add_subdirectory(), we can now use the gRPC targets directly from this build.
    set (_PROTOBUF_LIBPROTOBUF libprotobuf)
    set (_REFLECTION grpc++_reflection)
    set (_ORCA_SERVICE grpcpp_orca_service)
    if (CMAKE_CROSSCOMPILING)
        find_program (_PROTOBUF_PROTOC protoc)
    else ()
        set (_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
    endif ()
    set (_GRPC_GRPCPP grpc++)
    if (CMAKE_CROSSCOMPILING)
        find_program (_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
    else ()
        set (_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)
    endif ()
elseif (GRPC_FETCHCONTENT)
    # Another way is to use CMake's FetchContent module to clone gRPC at configure time. This makes
    # gRPC's source code available to your project, similar to a Git submodule.
    message (STATUS "Using gRPC via add_subdirectory (FetchContent).")
    include (FetchContent)
    FetchContent_Declare (
        grpc
        GIT_REPOSITORY https://github.com/grpc/grpc.git
        # When using gRPC, set this to an official gRPC release tag.
        # For testing purposes, the tag is overridden to point to the specific commit under test.
        GIT_TAG        vGRPC_TAG_VERSION_OF_YOUR_CHOICE)
    FetchContent_MakeAvailable (grpc)

    # Since FetchContent internally uses add_subdirectory(), the gRPC targets become directly
    # available for use in this build.
    set (_PROTOBUF_LIBPROTOBUF libprotobuf)
    set (_REFLECTION grpc++_reflection)
    set (_PROTOBUF_PROTOC $<TARGET_FILE:protoc>)
    set (_GRPC_GRPCPP grpc++)
    if (CMAKE_CROSSCOMPILING)
        find_program (_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
    else ()
        set (_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)
    endif ()
else ()
    # This branch assumes that gRPC and all its dependencies are already installed
    # on this system, so they can be located by find_package().

    # Find Protobuf installation.
    # Looks for the protobuf-config.cmake file installed by Protobuf's CMake installation.
    if (NOT KOS)
        set (protobuf_MODULE_COMPATIBLE TRUE)
    endif ()
    find_package (Protobuf CONFIG REQUIRED)
    message (STATUS "Using protobuf ${Protobuf_VERSION}")

    set (_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
    set (_REFLECTION gRPC::grpc++_reflection)
    if (CMAKE_CROSSCOMPILING)
        find_program (_PROTOBUF_PROTOC protoc)
    else ()
        set (_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
    endif ()

    # Find gRPC installation.
    # Looks for the gRPCConfig.cmake file installed by gRPC's CMake installation.
    find_package (gRPC CONFIG REQUIRED)
    message (STATUS "Using gRPC ${gRPC_VERSION}")

    set (_GRPC_GRPCPP gRPC::grpc++)
    if (CMAKE_CROSSCOMPILING)
        find_program (_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
    else ()
        set (_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
    endif ()
endif ()
