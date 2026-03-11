# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

# Toolchain for working with the NK-parser.
include (platform/nk)

# Creates a CMake target for generating an .edl.h file for one defined EDL file using the NK
# compiler.
nk_build_edl_files (client_edl_files NK_MODULE "wpa" EDL "${NK_RESOURCES}/wpa/Client.edl")

add_executable (Client "src/main.cpp" "src/connection_manager.cpp" ${EDL_FILES})
add_dependencies (Client client_edl_files)

find_package (Threads REQUIRED)

target_link_libraries (Client
    vfs::client
    wpa::wpa_ipc_client
    ${Boost_LIBRARIES}
)
