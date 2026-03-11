# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

# Include the CMake library named image, which contains the solution image build scripts.
include (platform/image)

set (SECURITY_PSL_FILE   "src/security.psl")
set (CONNECTION_CFG_FILE "src/init.yaml.in")
set (GINGER_DEFINITIONS  "BOARD=${CMAKE_SYSTEM_BOARD}")

# Define an ENTITIES variable with a list of programs.
set (ENTITIES
     Client
     precompiled_vfs::VfsNet
     rump::dhcpcd_entity)

# Set the arguments for the VFS program.
set (VFS_NET_MOUNT_FS "
    - -l
    - devfs /dev devfs rw
    - -l
    - ramfs /tmp ramfs rw
    - -l
    - ramfs /etc ramfs rw
    - -l
    - ramfs /var ramfs rw
    - -l
    - ramfs /var/db ramfs rw
    - -l
    - ramfs /var/db/dhcpcd ramfs rw
    - -l
    - romfs /romfs romfs ro")

set_target_properties (precompiled_vfs::VfsNet PROPERTIES
                       EXTRA_ARGS "${VFS_NET_MOUNT_FS}")

# Configure wpa_supplicant program and connections.
set_target_properties (wpa::supplicant PROPERTIES
    DEPENDS_ON_ENTITY ""
    EXTRA_CONNECTIONS "\
  - target: kl.VfsNet
    id: kl.VfsNet"
    EXTRA_ENV "\
    VFS_FILESYSTEM_BACKEND: client:kl.VfsNet
    VFS_NETWORK_BACKEND: client:kl.VfsNet"
    EXTRA_ARGS "\
    - '-iwl0'
    - '-c/romfs/wpa_supplicant.conf'")

# Configure Dhcpcd program and connections.
set_target_properties (rump::dhcpcd_entity PROPERTIES
    DEPENDS_ON_ENTITY ""
    EXTRA_CONNECTIONS "\
  - target: kl.VfsNet
    id: kl.VfsNet"
    EXTRA_ENV "\
    VFS_FILESYSTEM_BACKEND: client:kl.VfsNet
    VFS_NETWORK_BACKEND: client:kl.VfsNet"
    EXTRA_ARGS "\
    - '-4'
    - '-f/romfs/dhcpcd.conf'")

set_target_properties (precompiled_vfs::VfsNet PROPERTIES
    DEPENDS_ON_ENTITY "entropy::entity;dnet_brcm_fmac::executable;dnet_executables::whole")

set (FILES ${CMAKE_SOURCE_DIR}/resources/hdd/etc/wpa_supplicant.conf
           ${CMAKE_SOURCE_DIR}/resources/hdd/etc/dhcpcd.conf
           ${CMAKE_SOURCE_DIR}/resources/hdd/etc/client.conf)

if (${CMAKE_SYSTEM_BOARD} STREQUAL "RPI4_BCM2711")
    find_file (FW_BIN NAMES brcmfmac43455-sdio.bin PATHS /lib/firmware/brcm REQUIRED)
    find_file (FW_CLM NAMES brcmfmac43455-sdio.clm_blob PATHS /lib/firmware/brcm REQUIRED)
    find_file (FW_NV  NAMES brcmfmac43455-sdio.raspberrypi,4-model-b.txt PATHS /lib/firmware/brcm REQUIRED)
    add_custom_command (OUTPUT  ${CMAKE_BINARY_DIR}/brcmfmac43455-sdio.txt
                        COMMAND cp ${FW_NV} ${CMAKE_BINARY_DIR}/brcmfmac43455-sdio.txt)
    set (FW_NV "${CMAKE_BINARY_DIR}/brcmfmac43455-sdio.txt")
elseif (${CMAKE_SYSTEM_BOARD} STREQUAL "RADXA_ROCK_3A")
    find_file (FW_BIN NAMES brcmfmac43456-sdio.bin PATHS /lib/firmware/brcm REQUIRED)
    find_file (FW_CLM NAMES brcmfmac43456-sdio.clm_blob PATHS /lib/firmware/brcm REQUIRED)
    find_file (FW_NV  NAMES brcmfmac43456-sdio.AP6256.txt PATHS /lib/firmware/brcm REQUIRED)
    add_custom_command (OUTPUT  ${CMAKE_BINARY_DIR}/brcmfmac43456-sdio.txt
                        COMMAND cp ${FW_NV} ${CMAKE_BINARY_DIR}/brcmfmac43456-sdio.txt)
    set (FW_NV "${CMAKE_BINARY_DIR}/brcmfmac43456-sdio.txt")
else ()
    message (FATAL_ERROR "The board '${CMAKE_SYSTEM_BOARD}' is not supported.")
endif ()

set (FIRMWARE_FILES ${FW_BIN} ${FW_CLM} ${FW_NV})

if (NOT CMAKE_SYSTEM_BOARD STREQUAL "QEMU")
    # Create a top-level CMake target named kos-image which can be used when building a solution
    # image to be started on the hardware platform.
    build_kos_hw_image (kos-image
                        EINIT_ENTITY       EinitHw
                        CONNECTIONS_CFG    ${CONNECTION_CFG_FILE}
                        SECURITY_PSL       ${SECURITY_PSL_FILE}
                        GINGER_ENABLE      TRUE
                        GINGER_DEFINITIONS ${GINGER_DEFINITIONS}
                        IMAGE_FILES        ${ENTITIES} ${FIRMWARE_FILES} ${FILES})

    # Create a top-level CMake target named sd-image which can be used when building an SD card
    # image to be started on the hardware platform.
    include (${KL_SDK_ROOT_PATH}/common/build-sd-image.cmake)
    build_sd_image (sd-image
                    KOS_IMAGE_TARGET  kos-image
                    IMAGE_FS          ${IMAGE_FS})
endif ()
