# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

# Creates a CMake target for generating an .edl.h file for one defined EDL file using the NK
# compiler.
nk_build_edl_files (server_edl_files NK_MODULE "iperf" EDL "${NK_RESOURCES}/iperf/Server.edl")

add_executable (Server "src/main.c" ${EDL_FILES})
add_dependencies (Server server_edl_files)
target_link_libraries (Server vfs::client wpa::wpa_ipc_client iperf m)
target_compile_definitions (Server PUBLIC HAVE_STDATOMIC_H)

# Disable the default VFS entity that is automatically provided by vfs::client.
set_target_properties (Server PROPERTIES vfs_entity::entity_REPLACEMENT "")
