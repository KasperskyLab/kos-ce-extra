# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

project (launcher)

set (PROGRAM_NAME Launcher)
set (ENTITIES ${ENTITIES} ${PROGRAM_NAME} PARENT_SCOPE)

# Generate the *.edl.h file using the NK compiler for the Launcher program.
nk_build_edl_files (${PROGRAM_NAME}_edl_files
                    NK_MODULE "launcher"
                    EDL "${CMAKE_SOURCE_DIR}/resources/edl/Launcher.edl")

add_executable (${PROGRAM_NAME} "src/main.cpp")
add_dependencies (${PROGRAM_NAME} ${PROGRAM_NAME}_edl_files)
target_link_libraries (${PROGRAM_NAME} ${vfs_CLIENT_LIB}
                                       ${execution_manager_EXECMGR_PROXY})
# Set precompiled_vfsVfsSdCardFs as vfs_entity for the vfs_CLIENT_LIB.
set_target_properties (${PROGRAM_NAME}
                       PROPERTIES ${vfs_ENTITY}_REPLACEMENT ${precompiled_vfsVfsSdCardFs})

if (NOT DEFINED PACKAGE_ID OR ${PACKAGE_ID} STREQUAL "")
    message (FATAL_ERROR "PACKAGE_ID (ID of the installed KPA package) is empty! Set it in the CMakeLists.txt root file.")
endif ()
if (NOT DEFINED APP_INSTALL_DIR OR ${APP_INSTALL_DIR} STREQUAL "")
    message (FATAL_ERROR "APP_INSTALL_DIR (path where KPA packages are installed) is empty! Set it in the CMakeLists.txt root file.")
endif ()
target_compile_definitions (${PROGRAM_NAME} PUBLIC
                            PACKAGE_ID="${PACKAGE_ID}"
                            APP_INSTALL_DIR="${APP_INSTALL_DIR}")
