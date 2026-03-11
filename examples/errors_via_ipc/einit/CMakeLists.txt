# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

# Include the CMake library named image, which contains the solution image build scripts.
include (platform/image)

# Connect the CMake library named nk for working with the NK compiler (nk-gen-c).
include (platform/nk)

find_package (logrr_server REQUIRED)
find_package (logrr_fs_output_channel REQUIRED)

include (logrr_server/configure)
include (logrr_fs_output_channel/configure)

# Define the ENTITIES variable with a list of programs.
set (ENTITIES
     ServerC
     ClientC
     ServerCXX
     ClientCXX
     precompiled_vfs::VfsSdCardFs
     logrr_server::default_entity
     logrr_fs_output_channel::default_entity)

# The client is initialized with the INFO or DEBUG log level, based on the NDEBUG value. After
# establishing a connection to the server, its log level is synchronized with the configuration
# obtained from the server. It is possible to configure the client's log level for use when it is
# not yet connected to the server. To do this, set the INITIAL_LOG_LEVEL symbolic constant to one of
# the allowed values for LOG_LEVEL: CRITICAL, ERROR, WARNING, INFO, DEBUG, TRACE.
if (DEFINED INITIAL_LOG_LEVEL)
    set (LOG_LEVEL ${INITIAL_LOG_LEVEL})
else ()
    set (LOG_LEVEL INFO)
    if (CMAKE_BUILD_TYPE STREQUAL Debug)
        set (LOG_LEVEL DEBUG)
    endif ()
endif ()

# Add the server that uses the output channel FsOutputChannel.
create_logrr_server (VFS_CONN_ID "kl.VfsSdCardFs"
                     OUTPUT_CHANNELS "logrr.FsOutputChannel"
                     LOG_LEVEL ${LOG_LEVEL}
                     RECV_CORE_LOGS
                     WRITE_TO_CORE ON)

# Add the FsOutputChannel that writes to the file.
create_logrr_fs_output_channel (VFS_CONN_ID "kl.VfsSdCardFs"
                                LOG_DIR "/var/log")

# Create a static connection to the log server.
connect_to_logrr_server (ENTITIES ServerC ClientC ServerCXX ClientCXX)

set (SECURITY_PSL_FILE    "src/security.psl")
set (CONNECTIONS_CFG_FILE "src/init.yaml.in")
set (QEMU_DISK_IMAGE         sdcard0.img)
set (QEMU_DEPENDENCIES       ${QEMU_DISK_IMAGE})
set (QEMU_FLAGS              "-drive file=${QEMU_DISK_IMAGE},if=sd,format=raw")
set (QEMU_USER_NETWORK_FLAGS "hostfwd=tcp::1106-:1106")
set (GINGER_DEFINITIONS "BOARD=${CMAKE_SYSTEM_BOARD}")

if (NOT "${CMAKE_SYSTEM_BOARD}" STREQUAL "QEMU")
    # Create a top-level CMake target named kos-image which can be used when building a solution
    # image to be started on the hardware platform.
    build_kos_hw_image (kos-image
                        EINIT_ENTITY          EinitHw
                        CONNECTIONS_CFG       ${CONNECTIONS_CFG_FILE}
                        SECURITY_PSL          ${SECURITY_PSL_FILE}
                        GINGER_ENABLE         TRUE
                        GINGER_DEFINITIONS    ${GINGER_DEFINITIONS}
                        IMAGE_FILES           ${ENTITIES})

    # Create a top-level CMake target named sd-image which can be used when building an SD card
    # image to be started on the hardware platform.
    include (${KL_SDK_ROOT_PATH}/common/build-sd-image.cmake)
    build_sd_image (sd-image
                    KOS_IMAGE_TARGET kos-image)
else ()
    # Add a CMake target to create disk image for QEMU.
    add_custom_target (${QEMU_DISK_IMAGE}
                       COMMAND ${KL_SDK_ROOT_PATH}/common/prepare_hdd_img.sh
                               -s 64 -f fat32 -img ${QEMU_DISK_IMAGE})

    # Create a top-level CMake target named kos-qemu-image which can be used when building a
    # solution image to be started in QEMU.
    build_kos_qemu_image (kos-qemu-image
                          EINIT_ENTITY            EinitQemu
                          CONNECTIONS_CFG         ${CONNECTIONS_CFG_FILE}
                          SECURITY_PSL            ${SECURITY_PSL_FILE}
                          GINGER_ENABLE           TRUE
                          GINGER_DEFINITIONS      ${GINGER_DEFINITIONS}
                          QEMU_FLAGS              "${QEMU_FLAGS}"
                          QEMU_DEPENDENCIES       ${QEMU_DEPENDENCIES}
                          IMAGE_FILES             ${ENTITIES})
endif ()
