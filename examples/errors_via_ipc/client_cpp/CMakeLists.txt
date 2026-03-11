# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

# Create a CMake target for generating a *.edl.cpp.h file for a defined EDL file using the nkppmeta
# compiler.
add_nk_edl (CLIENT_CXX_EDL "${NK_RESOURCES}/errors_via_ipc_example/Client.edl"
            NK_MODULE      "errors_via_ipc_example"
            LANG           "CXX")

# Bind the CMake target to the main CMake target for building the application.
bind_nk_targets (CLIENT_CXX_EDL NUMBER_SUMMATION_IDL_CXX)

add_executable (ClientCXX "src/client.cpp")
target_link_libraries(ClientCXX PRIVATE logrr_cpp::logrr_cpp-static)
target_include_directories(ClientCXX PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../include/")
target_include_directories (ClientCXX PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../include/cpp")

bind_nk_targets (ClientCXX CLIENT_CXX_EDL)
