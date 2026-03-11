# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

set (PROGRAM_NAME Application)

# Creates a CMake target for generating an .edl.h file
# for one defined EDL file using the NK compiler.
nk_build_edl_files (${PROGRAM_NAME}_edl_files
                    IDL_TARGET Client_idl_files
                    NK_MODULE  "application"
                    EDL        "${NK_RESOURCES}/application/Application.edl")

add_executable (${PROGRAM_NAME} "src/main.cpp")
add_dependencies (${PROGRAM_NAME} ${PROGRAM_NAME}_edl_files)

target_link_libraries (${PROGRAM_NAME} vfs::client)
# Configure the VFS client to use the precompiled SD card file system implementation.
# This replaces the default VFS entity with precompiled_vfs::VfsSdCardFs.
set_target_properties (${PROGRAM_NAME} PROPERTIES
                       vfs_entity::entity_REPLACEMENT precompiled_vfs::VfsSdCardFs)
