# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

set (PROGRAM_NAME Client)
set (ENTITIES ${ENTITIES} ${PROGRAM_NAME} PARENT_SCOPE)

# Create a CMake target for generating .idl.h files
# for one or more defined IDL files using the NK compiler.
nk_build_idl_files (${PROGRAM_NAME}_idl_files
                    NK_MODULE "client"
                    IDL       "${NK_RESOURCES}/client/Notification.idl")

# Creates a CMake target for generating an .edl.h file
# for one defined EDL file using the NK compiler.
nk_build_edl_files (${PROGRAM_NAME}_edl_files
                    IDL_TARGET ${PROGRAM_NAME}_idl_files
                    NK_MODULE  "client"
                    EDL        "${NK_RESOURCES}/client/Client.edl")

add_executable (${PROGRAM_NAME} "src/main.cpp" "src/notification.cpp")
add_dependencies (${PROGRAM_NAME} ${PROGRAM_NAME}_edl_files)
target_link_libraries (${PROGRAM_NAME} vfs::client
                                       execution_manager::execution_manager_proxy)
# Configure the VFS client to use the precompiled SD card file system implementation.
# This replaces the default VFS entity with precompiled_vfs::VfsSdCardFs.
set_target_properties (${PROGRAM_NAME} PROPERTIES
                       vfs_entity::entity_REPLACEMENT precompiled_vfs::VfsSdCardFs)
