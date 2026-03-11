# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

# Include the CMake library named image
# containing the solution image build scripts.
include (platform/image)

find_package (logrr_server REQUIRED)
find_package (logrr_fs_output_channel REQUIRED)

include (logrr_server/configure)
include (logrr_fs_output_channel/configure)

# Define an ENTITIES variable with a list of programs.
set (ENTITIES
     precompiled_vfs::VfsSdCardFs
     precompiled_vfs::VfsNet
     rump::dhcpcd_entity
     logrr_server::default_entity
     logrr_fs_output_channel::default_entity
     WebServer
     TokenService)

set (KOS_LOG_LEVEL_DEFAULT INFO)
if (CMAKE_BUILD_TYPE STREQUAL Debug)
    set (KOS_LOG_LEVEL_DEFAULT DEBUG)
endif ()

set (KOS_LOG_LEVEL "${KOS_LOG_LEVEL_DEFAULT}" CACHE STRING "System log level")

# Create and configure the log server.
create_logrr_server (LOG_LEVEL       ${KOS_LOG_LEVEL}
                     RECV_CORE_LOGS  ${LOGRR_RECV_CORE_LOGS}
                     WRITE_TO_CORE   ON
                     OUTPUT_CHANNELS logrr.FsOutputChannel)

# Create and configure the channel for printing logs to files.
create_logrr_fs_output_channel (LOG_DIR "/log")

set (LOGRR_ENV "    VFS_FILESYSTEM_BACKEND: client:kl.VfsSdCardFs")
get_target_property (LOGRR_SERVER_TARGET logrr_server::default_entity ALIASED_TARGET)
set_target_properties (${LOGRR_SERVER_TARGET} PROPERTIES
                       DEPENDS_ON_ENTITY "precompiled_vfs::VfsSdCardFs"
                       EXTRA_ENV ${LOGRR_ENV}
                       EXTRA_CONNECTIONS "  - target: logrr.FsOutputChannel\n    id: logrr.FsOutputChannel\n "
                       vfs_entity::entity_REPLACEMENT precompiled_vfs::VfsSdCardFs)

get_target_property (LOGRR_FS_OUTPUT_TARGET logrr_fs_output_channel::default_entity ALIASED_TARGET)
get_target_property (__DEPENDS_ON_ENTITY_FS_OUTPUT ${LOGRR_FS_OUTPUT_TARGET} DEPENDS_ON_ENTITY)
list (APPEND __DEPENDS_ON_ENTITY_FS_OUTPUT precompiled_vfs::VfsSdCardFs)

set_target_properties (${LOGRR_FS_OUTPUT_TARGET} PROPERTIES
                       DEPENDS_ON_ENTITY "${__DEPENDS_ON_ENTITY_FS_OUTPUT}"
                       EXTRA_ENV ${LOGRR_ENV}
                       EXTRA_CONNECTIONS "  - target: logrr.Server\n    id: logrr.Server\n "
                       vfs_entity::entity_REPLACEMENT precompiled_vfs::VfsSdCardFs)

# Create static IPC channels for source programs to connect to the server.
connect_to_logrr_server (ENTITIES WebServer
                                  TokenService)

set (INIT_YAML_FILE          "src/init.yaml.in")
set (SECURITY_PSL_FILE       "src/security.psl")
set (GINGER_DEFINITIONS      "BOARD=${CMAKE_SYSTEM_BOARD}")
set (IMAGE_FS                "${CMAKE_BINARY_DIR}/hdd")
set (LIBS_PATH               "${IMAGE_FS}/lib")
set (QEMU_DISK_IMAGE         sdcard0.img)
set (QEMU_DEPENDENCIES       ${QEMU_DISK_IMAGE})
set (QEMU_FLAGS              "-drive file=${QEMU_DISK_IMAGE},if=sd,format=raw")
set (QEMU_USER_NETWORK_FLAGS "hostfwd=tcp::1106-:1106")

if (PLATFORM_SUPPORTS_DYNAMIC_LINKING)
    set_target_properties (blob_container::entity PROPERTIES
                           vfs_entity::entity_REPLACEMENT ""
                           DEPENDS_ON_ENTITY "precompiled_vfs::VfsSdCardFs"
                           EXTRA_ENV         "    VFS_FILESYSTEM_BACKEND: client:kl.VfsSdCardFs")
endif ()

set_target_properties (rump::dhcpcd_entity PROPERTIES
                       vfs_entity::entity_REPLACEMENT ""
                       DEPENDS_ON_ENTITY "precompiled_vfs::VfsSdCardFs;precompiled_vfs::VfsNet"
                       EXTRA_ENV "\
    VFS_FILESYSTEM_BACKEND: client{fs->net}:kl.VfsSdCardFs
    VFS_NETWORK_BACKEND: client:kl.VfsNet"
                       EXTRA_ARGS "\
    - '-4'
    - -f
    - /etc/dhcpcd.conf")

set_target_properties (precompiled_vfs::VfsNet PROPERTIES
                       EXTRA_ARGS "\
    - -l
    - devfs /dev devfs 0
    - -l
    - romfs /etc romfs ro")

# Add the hosts file to the ROMFS image for use by application.
set (FILES ${RESOURCES}/hdd/etc/hosts)

# Add a CMake target named disk_image_fs to create filesystem for disk image.
add_custom_target (disk_image_fs
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${RESOURCES}/hdd ${IMAGE_FS})

if (NOT "${CMAKE_SYSTEM_BOARD}" STREQUAL "QEMU")
    # Create a top-level CMake target named kos-image which can be used
    # when building a solution image to be started on the hardware platform.
    build_kos_hw_image (kos-image
                        EINIT_ENTITY          EinitHw
                        CONNECTIONS_CFG       ${INIT_YAML_FILE}
                        SECURITY_PSL          ${SECURITY_PSL_FILE}
                        GINGER_ENABLE         TRUE
                        GINGER_DEFINITIONS    ${GINGER_DEFINITIONS}
                        PACK_DEPS_COPY_ONLY   ON
                        PACK_DEPS_LIBS_PATH   ${LIBS_PATH}
                        PACK_DEPS_COPY_TARGET copylibs_hw
                        IMAGE_FILES           ${ENTITIES} ${FILES})

    add_dependencies (kos-image disk_image_fs copylibs_hw)

    # Create a top-level CMake target named sd-image which can be used
    # when building an SD card image to be started on the hardware platform.
    include (${KL_SDK_ROOT_PATH}/common/build-sd-image.cmake)
    build_sd_image (sd-image
                    KOS_IMAGE_TARGET kos-image
                    IMAGE_FS ${IMAGE_FS})
else ()
    # Add a CMake target to create disk image for QEMU.
    add_custom_target (${QEMU_DISK_IMAGE}
                       DEPENDS disk_image_fs copylibs_qemu
                       COMMAND ${KL_SDK_ROOT_PATH}/common/prepare_hdd_img.sh
                               -d ${IMAGE_FS} -s 64 -f fat32 -img ${QEMU_DISK_IMAGE})

    # Create a top-level CMake target named kos-qemu-image which
    # can be used when building a solution image to be started in QEMU.
    build_kos_qemu_image (kos-qemu-image
                          EINIT_ENTITY            EinitQemu
                          CONNECTIONS_CFG         ${INIT_YAML_FILE}
                          SECURITY_PSL            ${SECURITY_PSL_FILE}
                          GINGER_ENABLE           TRUE
                          GINGER_DEFINITIONS      ${GINGER_DEFINITIONS}
                          QEMU_FLAGS              "${QEMU_FLAGS}"
                          QEMU_USER_NETWORK_FLAGS "${QEMU_USER_NETWORK_FLAGS}"
                          QEMU_DEPENDENCIES       ${QEMU_DEPENDENCIES}
                          PACK_DEPS_COPY_ONLY     ON
                          PACK_DEPS_LIBS_PATH     ${LIBS_PATH}
                          PACK_DEPS_COPY_TARGET   copylibs_qemu
                          IMAGE_FILES             ${ENTITIES} ${FILES})
endif ()
