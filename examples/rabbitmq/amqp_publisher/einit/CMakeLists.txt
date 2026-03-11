# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

# Include the CMake library named image containing the solution image build scripts.
include (platform/image)

find_program (NTPD_ENTITY
              NAMES "ntpd" REQUIRED
              NO_CMAKE_PATH
              NO_CMAKE_ENVIRONMENT_PATH
              NO_SYSTEM_ENVIRONMENT_PATH)

# Define an ENTITIES variable with a list of programs.
set (ENTITIES
     Publisher
     precompiled_vfs::VfsSdCardFs
     precompiled_vfs::VfsNet
     ${NTPD_ENTITY}
     rump::dhcpcd_entity)

set (INIT_YAML_FILE     "src/init.yaml.in")
set (SECURITY_PSL_FILE  "src/security.psl")
set (GINGER_DEFINITIONS "BOARD=${CMAKE_SYSTEM_BOARD}")
set (IMAGE_FS           "${CMAKE_BINARY_DIR}/hdd")
set (LIBS_PATH          "${IMAGE_FS}/lib")
set (ROOTFS_IMAGE       "sdcard0.img")
set (QEMU_FLAGS         "-drive file=${ROOTFS_IMAGE},if=sd,format=raw")
set (QEMU_DEPENDENCIES  ${ROOTFS_IMAGE})
# Add host configuration files to ROMFS image for system programs.
set (FILES              ${RESOURCES}/hdd/etc/hosts
                        ${RESOURCES}/hdd/etc/dhcpcd.conf
                        ${RESOURCES}/hdd/etc/ntp.conf)

if (PLATFORM_SUPPORTS_DYNAMIC_LINKING)
    set_target_properties (blob_container::entity PROPERTIES
                           vfs_entity::entity_REPLACEMENT ""
                           DEPENDS_ON_ENTITY precompiled_vfs::VfsSdCardFs
                           EXTRA_ENV         "    VFS_FILESYSTEM_BACKEND: client:kl.VfsSdCardFs")
endif()

set (DHCPCD_ENV "
     VFS_FILESYSTEM_BACKEND: client{fs->net}:kl.VfsSdCardFs
     VFS_NETWORK_BACKEND: client:kl.VfsNet")

set (DHCPCD_ARGS "
    - '-4'
    - -f
    - /etc/dhcpcd.conf")

set_target_properties (rump::dhcpcd_entity PROPERTIES
                       DEPENDS_ON_ENTITY "precompiled_vfs::VfsSdCardFs;precompiled_vfs::VfsNet"
                       EXTRA_ENV         ${DHCPCD_ENV}
                       EXTRA_ARGS        ${DHCPCD_ARGS})

set_target_properties (precompiled_vfs::VfsSdCardFs PROPERTIES
                       EXTRA_ARGS "\
    - -l
    - romfs /romfs romfs ro
    - -l
    - nodev /var ramfs 0
    - -l
    - devfs /dev devfs 0")

set_target_properties (precompiled_vfs::VfsNet PROPERTIES
                       EXTRA_ARGS "\
    - -l
    - devfs /dev devfs 0
    - -l
    - romfs /etc romfs ro")

# Add a target to create filesystem for disk image.
add_custom_target (disk_image_fs
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${RESOURCES}/hdd ${IMAGE_FS})

# Add a target to create disk image.
add_custom_target (${ROOTFS_IMAGE}
    DEPENDS disk_image_fs copylibs_qemu
    COMMAND ${KL_SDK_ROOT_PATH}/common/prepare_hdd_img.sh
            -d ${IMAGE_FS} -s 64 -f fat32 -img ${ROOTFS_IMAGE})

if (NOT CMAKE_SYSTEM_BOARD STREQUAL "QEMU")
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
                    IMAGE_FS         ${IMAGE_FS})

else ()
    # Create a top-level CMake target named kos-qemu-image which
    # can be used when building a solution image to be started in QEMU.
    build_kos_qemu_image (kos-qemu-image
                          EINIT_ENTITY          EinitQemu
                          CONNECTIONS_CFG       ${INIT_YAML_FILE}
                          SECURITY_PSL          ${SECURITY_PSL_FILE}
                          GINGER_ENABLE         TRUE
                          GINGER_DEFINITIONS    ${GINGER_DEFINITIONS}
                          QEMU_FLAGS            "${QEMU_FLAGS}"
                          QEMU_DEPENDENCIES     ${QEMU_DEPENDENCIES}
                          PACK_DEPS_COPY_ONLY   ON
                          PACK_DEPS_LIBS_PATH   ${LIBS_PATH}
                          PACK_DEPS_COPY_TARGET copylibs_qemu
                          IMAGE_FILES           ${ENTITIES} ${FILES})
endif ()
