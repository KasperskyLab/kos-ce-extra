# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

set (PROGRAM_NAME PkgMgrEntity)
set (ENTITIES     ${ENTITIES} ${PROGRAM_NAME} PARENT_SCOPE)

# Build the PackageManager component for this solution using the
# create_package_manager_entity() CMake command from the package_manager
# CMake library.
include (package_manager/create_package_manager_entity)

create_package_manager_entity (ENTITY                       ${PROGRAM_NAME}
                               NK_MODULE_NAME               "package_manager"
                               MAIN_CONN_NAME               "${PROGRAM_NAME}"
                               ROOT_PATH                    "/"
                               PKGS_DIR                     "/packages"
                               PKG_EXTENSION                "kpa"
                               DB_PATH                      "${PKG_DB_PATH}"
                               APPS_DIR                     "/${APP_INSTALL_DIR}"
                               MANIFEST_SCHEMA_BUILD_STORE  "${ROOTFS_DIR}/schema"
                               MANIFEST_SCHEMA_RUNTIME_PATH "/schema"
                               VFS_CLIENT_LIB               vfs::client)

# Configure the VFS client to use the precompiled SD card file system implementation.
# This replaces the default VFS entity with precompiled_vfs::VfsSdCardFs.
set_target_properties (${PROGRAM_NAME} PROPERTIES
                       vfs:entity_REPLACEMENT precompiled_vfs::VfsSdCardFs
                       EXTRA_ENV              "    VFS_FILESYSTEM_BACKEND: client:kl.VfsSdCardFs")
