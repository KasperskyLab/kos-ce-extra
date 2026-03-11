# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

# Generate an EDL file containing a description of the process class.
generate_edl_file (Client)
# Create a CMake target for generating an .edl.h file for one defined EDL file using the NK
# compiler.
nk_build_edl_files (client_edl_files NK_MODULE "mbim_sms" EDL ${EDL_FILE})

add_executable (Client "src/main.c")
add_dependencies (Client client_edl_files)

target_link_libraries (Client vfs::client)

# Disable the default VFS entity that is automatically provided by vfs::client.
set_target_properties (Client PROPERTIES vfs_entity::entity_REPLACEMENT "")
