# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

# Create a CMake target for generating a *.edl.cpp.h file for a defined EDL file using the nkppmeta
# compiler.
add_nk_edl (CLIENT_C_EDL "${NK_RESOURCES}/errors_via_ipc_example/Client.edl"
            NK_MODULE    "errors_via_ipc_example"
            LANG         "C")

# Bind the CMake target to the main CMake target for building the application.
bind_nk_targets (CLIENT_C_EDL NUMBER_SUMMATION_IDL_C)

add_executable (ClientC "src/client.c")
target_link_libraries (ClientC PRIVATE logrr_clog::logrr_clog-static)
target_include_directories(ClientC PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../include/")
target_include_directories(ClientC PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../include/c")
bind_nk_targets (ClientC CLIENT_C_EDL)
