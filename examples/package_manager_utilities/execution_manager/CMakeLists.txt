# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

set (PROGRAM_NAME ExecMgrEntity)
set (ENTITIES ${ENTITIES} ${PROGRAM_NAME} PARENT_SCOPE)

# Build the ExecutionManager component for this solution using the
# create_execution_manager_entity() CMake command from the execution_manager
# CMake library.
include (execution_manager/create_execution_manager_entity)

create_execution_manager_entity (ENTITY         ${PROGRAM_NAME}
                                 NK_MODULE_NAME "execution_manager"
                                 MAIN_CONN_NAME ${PROGRAM_NAME}
                                 ROOT_PATH      "/"
                                 VFS_CLIENT_LIB vfs::client)

# Configure the VFS client to use the precompiled SD card file system implementation.
# This replaces the default VFS entity with precompiled_vfs::VfsSdCardFs.
set_target_properties (${PROGRAM_NAME} PROPERTIES
                       vfs_entity::entity_REPLACEMENT precompiled_vfs::VfsSdCardFs
                       EXTRA_ENV               "    VFS_FILESYSTEM_BACKEND: client:kl.VfsSdCardFs")
