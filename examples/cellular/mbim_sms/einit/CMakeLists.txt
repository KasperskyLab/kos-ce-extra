# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

# Include the CMake library named image, which contains the solution image build scripts.
include (platform/image)

set (SECURITY_PSL_FILE    "src/security.psl")
set (CONNECTIONS_CFG_FILE "src/init.yaml.in")
set (GINGER_DEFINITIONS   "BOARD=${CMAKE_SYSTEM_BOARD}")

# Define an ENTITIES variable with a list of programs.
set (ENTITIES Client precompiled_vfs::VfsNet)

# Change DNet driver provider to the MBIM driver.
set_target_properties (precompiled_vfs::VfsNet PROPERTIES
                       DEPENDS_ON_ENTITY "entropy::entity;dnet_executables::mbim")

if (NOT CMAKE_SYSTEM_BOARD STREQUAL "QEMU")
    # Create a top-level CMake target named kos-image which can be used when building a solution
    # image to be started on the hardware platform.
    build_kos_hw_image (kos-image
                        EINIT_ENTITY       EinitHw
                        CONNECTIONS_CFG    ${CONNECTIONS_CFG_FILE}
                        SECURITY_PSL       ${SECURITY_PSL_FILE}
                        GINGER_ENABLE      TRUE
                        GINGER_DEFINITIONS ${GINGER_DEFINITIONS}
                        IMAGE_FILES        ${ENTITIES})

    # Create a top-level CMake target named sd-image which can be used when building an SD card
    # image to be started on the hardware platform.
    include (${KL_SDK_ROOT_PATH}/common/build-sd-image.cmake)
    build_sd_image (sd-image
                    KOS_IMAGE_TARGET kos-image)
endif ()
