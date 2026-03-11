# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

# Include the CMake library named image, which contains the solution image build scripts.
include (platform/image)

# Choose secure (ON) or insecure (OFF) connection for gRPC server.
option (USE_SECURE_CONNECTION "Use secure connection for gRPC")

set (CONNECTIONS_CFG_FILE "src/${BUILD_TARGET}.init.yaml.in")
set (SECURITY_PSL_FILE    "src/security.psl.in")
set (GINGER_DEFINITIONS   "BOARD=${CMAKE_SYSTEM_BOARD}")
set (IMAGE_FS             "${CMAKE_BINARY_DIR}/hdd")
set (LIBS_PATH            "${IMAGE_FS}/lib")
set (QEMU_DISK_IMAGE      sdcard0.img)
set (QEMU_DEPENDENCIES    ${QEMU_DISK_IMAGE})
set (QEMU_FLAGS           "-drive file=${QEMU_DISK_IMAGE},if=sd,format=raw")

if (${BUILD_TARGET} STREQUAL "server")
    set (QEMU_USER_NETWORK_FLAGS "hostfwd=tcp::50051-:50051")
endif ()

# Define the ENTITIES variable with a list of program.
set (ENTITIES
     ${MAIN_TASK}
     ${NTPD}
     precompiled_vfs::VfsSdCardFs
     precompiled_vfs::VfsNet
     rump::dhcpcd_entity)

set_target_properties (rump::dhcpcd_entity PROPERTIES
    DEPENDS_ON_ENTITY ""
    EXTRA_CONNECTIONS "
  - target: kl.VfsNet
    id: kl.VfsNet
  - target: kl.VfsSdCardFs
    id: kl.VfsSdCardFs"
    EXTRA_ENV "
    VFS_FILESYSTEM_BACKEND: client{fs->net}:kl.VfsSdCardFs
    VFS_NETWORK_BACKEND: client:kl.VfsNet"
    EXTRA_ARGS "
    - '-4'
    - -f
    - /etc/dhcpcd.conf")

set_target_properties (precompiled_vfs::VfsSdCardFs PROPERTIES
    EXTRA_ARGS "
    - -l
    - nodev /tmp ramfs 0
    - -l
    - nodev /var ramfs 0
    - -l
    - devfs /dev devfs 0")

if (PLATFORM_SUPPORTS_DYNAMIC_LINKING)
    set_target_properties(${blob_container_ENTITY} PROPERTIES
        DEPENDS_ON_ENTITY precompiled_vfs::VfsSdCardFs
        EXTRA_ENV "    VFS_FILESYSTEM_BACKEND: client:kl.VfsSdCardFs")
endif ()

if (USE_SECURE_CONNECTION)
    set_target_properties (${MAIN_TASK} PROPERTIES EXTRA_ARGS "    - --secure")
endif ()

# Add a target to create filesystem for disk image.
add_custom_target (disk_image_fs
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${RESOURCES}/hdd ${IMAGE_FS}
                   COMMAND ${CMAKE_COMMAND} -E make_directory ${IMAGE_FS}/cert
                   COMMAND ${CMAKE_COMMAND} -E copy ${SSL_CERTS} ${IMAGE_FS}/cert)

if (NOT "${CMAKE_SYSTEM_BOARD}" STREQUAL "QEMU")
    # Create a top-level CMake target named kos-image which can be used
    # when building a solution image to be started on the hardware platform.
    build_kos_hw_image (kos-image
                        EINIT_ENTITY          EinitHw
                        CONNECTIONS_CFG       "${CONNECTIONS_CFG_FILE}"
                        SECURITY_PSL          "${SECURITY_PSL_FILE}"
                        GINGER_ENABLE         TRUE
                        GINGER_DEFINITIONS    ${GINGER_DEFINITIONS}
                        PACK_DEPS_COPY_ONLY   ON
                        PACK_DEPS_LIBS_PATH   ${LIBS_PATH}
                        PACK_DEPS_COPY_TARGET copylibs_hw
                        IMAGE_FILES           ${ENTITIES})

    add_dependencies (kos-image disk_image_fs copylibs_hw)

    # Create a top-level CMake target named sd-image which can be used
    # when building an SD card image to be started on the hardware platform.
    include (${KL_SDK_ROOT_PATH}/common/build-sd-image.cmake)
    build_sd_image (sd-image
                    KOS_IMAGE_TARGET kos-image
                    IMAGE_FS ${IMAGE_FS})
else ()
    # Add a target to create disk image for QEMU.
    add_custom_target (${QEMU_DISK_IMAGE}
                       DEPENDS disk_image_fs copylibs_qemu
                       COMMAND ${KL_SDK_ROOT_PATH}/common/prepare_hdd_img.sh
                               -d ${IMAGE_FS} -s 128 -f fat32 -img ${QEMU_DISK_IMAGE})

    # Create a top-level CMake target named kos-qemu-image which can be used
    # when building a solution image to be started in QEMU.
    build_kos_qemu_image (kos-qemu-image
                          EINIT_ENTITY            EinitQemu
                          CONNECTIONS_CFG         "${CONNECTIONS_CFG_FILE}"
                          SECURITY_PSL            "${SECURITY_PSL_FILE}"
                          GINGER_ENABLE           TRUE
                          GINGER_DEFINITIONS      ${GINGER_DEFINITIONS}
                          QEMU_FLAGS              "${QEMU_FLAGS}"
                          QEMU_USER_NETWORK_FLAGS "${QEMU_USER_NETWORK_FLAGS}"
                          QEMU_DEPENDENCIES       ${QEMU_DEPENDENCIES}
                          PACK_DEPS_COPY_ONLY     ON
                          PACK_DEPS_LIBS_PATH     ${LIBS_PATH}
                          PACK_DEPS_COPY_TARGET   copylibs_qemu
                          IMAGE_FILES             ${ENTITIES})
endif ()
