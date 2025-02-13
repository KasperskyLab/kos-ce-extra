# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

project (application)

set (PROGRAM_NAME Application)

# Generate the *.edl.h file using the NK compiler for the program
# specified by the {PROGRAM_NAME}_edl_files variable.
nk_build_edl_files (${PROGRAM_NAME}_edl_files
                    IDL_TARGET Client_idl_files
                    NK_MODULE  "application"
                    EDL        "${CMAKE_SOURCE_DIR}/resources/edl/Application.edl")

add_executable (${PROGRAM_NAME} "src/main.cpp")
add_dependencies (${PROGRAM_NAME} ${PROGRAM_NAME}_edl_files)

target_link_libraries (${PROGRAM_NAME} ${vfs_CLIENT_LIB})
# Set precompiled_vfsVfsRamFs as vfs_entity for the vfs_CLIENT_LIB.
set_target_properties (${PROGRAM_NAME}
                       PROPERTIES ${vfs_ENTITY}_REPLACEMENT ${precompiled_vfsVfsRamFs})
