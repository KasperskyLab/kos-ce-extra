# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

# Create a CMake target for generating a *.idl.cpp.h header file for a defined IDL file using the
# nkppmeta compiler.
add_nk_idl (NUMBER_SUMMATION_IDL_CXX "${NK_RESOURCES}/errors_via_ipc_example/NumberSummation.idl"
            NK_MODULE                "errors_via_ipc_example"
            LANG                     "CXX")

# Create a CMake target for generating a *.edl.cpp.h file for a defined EDL file using the nkppmeta
# compiler.
add_nk_edl (SERVER_CXX_EDL "${NK_RESOURCES}/errors_via_ipc_example/Server.edl"
            NK_MODULE      "errors_via_ipc_example"
            LANG           "CXX")

# Bind the CMake target to the main CMake target for building the application.
bind_nk_targets (SERVER_CXX_EDL NUMBER_SUMMATION_IDL_CXX)

add_executable (ServerCXX "src/server.cpp")
target_link_libraries(ServerCXX PRIVATE logrr_cpp::logrr_cpp-static)
target_include_directories(ServerCXX PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../include/")
target_include_directories (ServerCXX PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../include/cpp")

bind_nk_targets (ServerCXX SERVER_CXX_EDL)
