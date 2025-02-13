# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

project (execution_manager)

set (PROGRAM_NAME ExecMgrEntity)
set (ENTITIES ${ENTITIES} ${PROGRAM_NAME} PARENT_SCOPE)

# Build the ExecutionManager component for this solution using the
# create_execution_manager_entity() CMake command from the execution_manager
# CMake library.
include (execution_manager/create_execution_manager_entity)

create_execution_manager_entity(ENTITY         ${PROGRAM_NAME}
                                NK_MODULE_NAME ${PROJECT_NAME}
                                MAIN_CONN_NAME ${PROGRAM_NAME}
                                ROOT_PATH      "/"
                                VFS_CLIENT_LIB ${vfs_CLIENT_LIB})

# Set precompiled_vfsVfsRamFs as vfs_entity for the vfs_CLIENT_LIB.
set_target_properties (${PROGRAM_NAME}
                       PROPERTIES ${vfs_ENTITY}_REPLACEMENT ${precompiled_vfsVfsRamFs}
                                  EXTRA_ENV                 "    VFS_FILESYSTEM_BACKEND: client:kl.VfsRamFs")
