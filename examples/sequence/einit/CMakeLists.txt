# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

# Include the CMake library named image, which contains the solution image build scripts.
include (platform/image)

# Connect the CMake library named nk for working with the NK compiler (nk-gen-c).
include (platform/nk)

set (SECURITY_PSL_FILE    "src/security.psl")
set (CONNECTIONS_CFG_FILE "src/init.yaml.in")
set (GINGER_DEFINITIONS   "BOARD=${CMAKE_SYSTEM_BOARD}")

# Define the ENTITIES variable with a list of programs.
set (ENTITIES
     Reader
     Writer)

if (NOT CMAKE_SYSTEM_BOARD STREQUAL "QEMU")
    # Create a top-level CMake target named kos-image which
    # can be used when building a solution image to be started on the hardware platform.
    build_kos_hw_image (kos-image
                        EINIT_ENTITY       EinitHw
                        CONNECTIONS_CFG    ${CONNECTIONS_CFG_FILE}
                        SECURITY_PSL       ${SECURITY_PSL_FILE}
                        GINGER_ENABLE      TRUE
                        GINGER_DEFINITIONS ${GINGER_DEFINITIONS}
                        IMAGE_FILES        ${ENTITIES})

    # Create a top-level CMake target named sd-image which
    # can be used when building an SD-card image to be started on the hardware platform.
    include (${KL_SDK_ROOT_PATH}/common/build-sd-image.cmake)
    build_sd_image (sd-image
                    KOS_IMAGE_TARGET kos-image)
else ()
    # Create a top-level CMake target named kos-qemu-image which
    # can be used when building a solution image to be started in QEMU.
    build_kos_qemu_image (kos-qemu-image
                          EINIT_ENTITY       EinitQemu
                          CONNECTIONS_CFG    ${CONNECTIONS_CFG_FILE}
                          SECURITY_PSL       ${SECURITY_PSL_FILE}
                          GINGER_ENABLE      TRUE
                          GINGER_DEFINITIONS ${GINGER_DEFINITIONS}
                          QEMU_FLAGS         "-nographic -monitor none"
                          IMAGE_FILES        ${ENTITIES})
endif ()
