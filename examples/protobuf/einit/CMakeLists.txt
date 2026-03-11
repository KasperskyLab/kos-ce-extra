# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

# Include the CMake library named image, which contains the solution image build scripts.
include (platform/image)

find_package (logrr_server REQUIRED)
find_package (logrr_fs_output_channel REQUIRED)

include (logrr_server/configure)
include (logrr_fs_output_channel/configure)

# Define an ENTITIES variable with a list of programs.
set (ENTITIES Consumer
              Producer
              precompiled_vfs::VfsSdCardFs
              logrr_server::default_entity
              logrr_fs_output_channel::default_entity)

if (DEFINED INITIAL_LOG_LEVEL)
    set (LOG_LEVEL ${INITIAL_LOG_LEVEL})
else ()
    set (LOG_LEVEL INFO)
    if (CMAKE_BUILD_TYPE STREQUAL Debug)
        set (LOG_LEVEL DEBUG)
    endif ()
endif ()

# Add the server that uses the output channel FsOutputChannel.
create_logrr_server (VFS_CONN_ID     "kl.VfsSdCardFs"
                     OUTPUT_CHANNELS "logrr.FsOutputChannel"
                     LOG_LEVEL       ${LOG_LEVEL}
                     WRITE_TO_CORE   ON
                     RECV_CORE_LOGS  ${LOGRR_RECV_CORE_LOGS})

# Add the FsOutputChannel that writes to the file.
create_logrr_fs_output_channel (VFS_CONN_ID "kl.VfsSdCardFs"
                                LOG_DIR     "/log")

# Create static IPC channels for source programs to connect to the server.
connect_to_logrr_server (ENTITIES Consumer
                                  Producer)

set (SECURITY_PSL_FILE    "src/security.psl")
set (CONNECTIONS_CFG_FILE "src/init.yaml.in")
set (IMAGE_FS             "${CMAKE_BINARY_DIR}/hdd")
set (LIBS_PATH            "${IMAGE_FS}/lib")
set (QEMU_DISK_IMAGE      sdcard0.img)
set (QEMU_DEPENDENCIES    ${QEMU_DISK_IMAGE})
set (QEMU_FLAGS           "-drive file=${QEMU_DISK_IMAGE},if=sd,format=raw")
set (GINGER_DEFINITIONS   "BOARD=${CMAKE_SYSTEM_BOARD}")

if (PLATFORM_SUPPORTS_DYNAMIC_LINKING)
    set_target_properties (blob_container::entity PROPERTIES
                           DEPENDS_ON_ENTITY "precompiled_vfs::VfsSdCardFs"
                           EXTRA_ENV "    VFS_FILESYSTEM_BACKEND: client:kl.VfsSdCardFs")
endif ()

# Add a target to create filesystem for disk image.
add_custom_target (disk_image_fs
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${RESOURCES}/hdd ${IMAGE_FS})

if (NOT "${CMAKE_SYSTEM_BOARD}" STREQUAL "QEMU")
    # Create a top-level CMake target named kos-image which can be used
    # when building a solution image to be started on the hardware platform.
    build_kos_hw_image (kos-image
                        EINIT_ENTITY          EinitHw
                        CONNECTIONS_CFG       ${CONNECTIONS_CFG_FILE}
                        SECURITY_PSL          ${SECURITY_PSL_FILE}
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
                    IMAGE_FS         ${IMAGE_FS})
else ()
    # Add a target to create disk image for QEMU.
    add_custom_target (${QEMU_DISK_IMAGE}
                       DEPENDS disk_image_fs copylibs_qemu
                       COMMAND ${KL_SDK_ROOT_PATH}/common/prepare_hdd_img.sh
                               -d ${IMAGE_FS} -s 64 -f fat32 -img ${QEMU_DISK_IMAGE})

    # Create a top-level CMake target named kos-qemu-image which
    # can be used when building a solution image to be started in QEMU.
    build_kos_qemu_image (kos-qemu-image
                          EINIT_ENTITY          EinitQemu
                          CONNECTIONS_CFG       ${CONNECTIONS_CFG_FILE}
                          SECURITY_PSL          ${SECURITY_PSL_FILE}
                          GINGER_ENABLE         TRUE
                          GINGER_DEFINITIONS    ${GINGER_DEFINITIONS}
                          QEMU_FLAGS            "${QEMU_FLAGS}"
                          QEMU_DEPENDENCIES     ${QEMU_DEPENDENCIES}
                          PACK_DEPS_COPY_ONLY   ON
                          PACK_DEPS_LIBS_PATH   ${LIBS_PATH}
                          PACK_DEPS_COPY_TARGET copylibs_qemu
                          IMAGE_FILES           ${ENTITIES})
endif ()
