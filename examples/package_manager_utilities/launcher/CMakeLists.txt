# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

set (PROGRAM_NAME Launcher)
set (ENTITIES ${ENTITIES} ${PROGRAM_NAME} PARENT_SCOPE)

# Generate the *.edl.h file using the NK compiler for the Launcher program.
nk_build_edl_files (${PROGRAM_NAME}_edl_files
                    NK_MODULE "launcher"
                    EDL       "${NK_RESOURCES}/launcher/Launcher.edl")

add_executable (${PROGRAM_NAME} "src/main.cpp")
add_dependencies (${PROGRAM_NAME} ${PROGRAM_NAME}_edl_files)

# Link the Launcher executable file to the client proxy library of PackageManager
# (package_manager::package_manager_proxy).
target_link_libraries (${PROGRAM_NAME} vfs::client
                                       package_manager::package_manager_proxy
                                       execution_manager::execution_manager_proxy)

# Configure the VFS client to use the precompiled SD card file system implementation.
# This replaces the default VFS entity with precompiled_vfs::VfsSdCardFs.
set_target_properties (${PROGRAM_NAME} PROPERTIES
                       vfs_entity::entity_REPLACEMENT precompiled_vfs::VfsSdCardFs)

if (NOT DEFINED PACKAGE_ID OR ${PACKAGE_ID} STREQUAL "")
    message (FATAL_ERROR "PACKAGE_ID (ID of the installed KPA package) is empty! Set it in the CMakeLists.txt root file.")
endif ()
target_compile_definitions (${PROGRAM_NAME} PUBLIC
                            PACKAGE_ID="${PACKAGE_ID}")
