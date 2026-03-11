# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

set (PROGRAM_NAME Application)

# Generate the *.edl.h file using the NK compiler for the Application program.
nk_build_edl_files (${PROGRAM_NAME}_edl_files
                    NK_MODULE "application"
                    EDL "${NK_RESOURCES}/application/Application.edl")

add_executable (${PROGRAM_NAME} "src/main.cpp")
add_dependencies (${PROGRAM_NAME} ${PROGRAM_NAME}_edl_files)

target_link_libraries (${PROGRAM_NAME} vfs::client)
# Disable the default VFS entity that is automatically provided by vfs::client.
set_target_properties (${PROGRAM_NAME} PROPERTIES
                       vfs_entity::entity_REPLACEMENT "")
