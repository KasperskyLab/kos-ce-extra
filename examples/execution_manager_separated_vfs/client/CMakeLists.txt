# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

project (client)

set (PROGRAM_NAME Client)
set (ENTITIES ${ENTITIES} ${PROGRAM_NAME} PARENT_SCOPE)

# Generate the *.idl.h file using the NK compiler for the program
# specified by the {PROGRAM_NAME}_idl_files variable.
nk_build_idl_files (${PROGRAM_NAME}_idl_files
                    NK_MODULE "client"
                    IDL       "${CMAKE_SOURCE_DIR}/resources/idl/Notification.idl")

nk_build_edl_files (${PROGRAM_NAME}_edl_files
                    IDL_TARGET ${PROGRAM_NAME}_idl_files
                    NK_MODULE  "client"
                    EDL        "${CMAKE_SOURCE_DIR}/resources/edl/Client.edl")

add_executable (${PROGRAM_NAME} "src/main.cpp" "src/notification.cpp")
add_dependencies (${PROGRAM_NAME} ${PROGRAM_NAME}_edl_files)
target_link_libraries (${PROGRAM_NAME} ${vfs_CLIENT_LIB}
                                       ${execution_manager_EXECMGR_PROXY})
# Set precompiled_vfsVfsRamFs as vfs_entity for the vfs_CLIENT_LIB.
set_target_properties (${PROGRAM_NAME}
                       PROPERTIES ${vfs_ENTITY}_REPLACEMENT ${precompiled_vfsVfsRamFs})
