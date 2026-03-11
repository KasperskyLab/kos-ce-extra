# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

# Create a CMake target for generating a *.idl.cpp.h header file for a defined IDL file using the
# nkppmeta compiler.
add_nk_idl (NUMBER_SUMMATION_IDL_C "${NK_RESOURCES}/errors_via_ipc_example/NumberSummation.idl"
            NK_MODULE              "errors_via_ipc_example"
            LANG                   "C")

# Create a CMake target for generating a *.edl.cpp.h file for a defined EDL file using the nkppmeta
# compiler.
add_nk_edl (SERVER_C_EDL "${NK_RESOURCES}/errors_via_ipc_example/Server.edl"
            NK_MODULE    "errors_via_ipc_example"
            LANG         "C")

# Bind the CMake target to the main CMake target for building the application.
bind_nk_targets (SERVER_C_EDL NUMBER_SUMMATION_IDL_C)

add_executable (ServerC "src/server.c")
target_link_libraries (ServerC PRIVATE logrr_clog::logrr_clog-static)
target_include_directories(ServerC PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../include/")
target_include_directories(ServerC PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../include/c")
bind_nk_targets (ServerC SERVER_C_EDL)
