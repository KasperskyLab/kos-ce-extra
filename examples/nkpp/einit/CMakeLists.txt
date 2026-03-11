# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

project (einit)

# Include the CMake library named image, which contains the solution image build scripts.
include (platform/image)

# Define the ENTITIES variable with a list of programs.
set (ENTITIES             ${ENTITIES})
set (CONNECTIONS_CFG_FILE "src/init.yaml.in")
set (SECURITY_PSL_FILE    "src/security.psl.in")
set (QEMU_FLAGS           "-nographic -monitor none")

if (NOT CMAKE_SYSTEM_BOARD STREQUAL "QEMU")
    # Create a top-level CMake target named kos-image which can be used
    # when building a solution image to be started on the hardware platform.
    build_kos_hw_image (kos-image
                        EINIT_ENTITY    EinitHw
                        CONNECTIONS_CFG ${CONNECTIONS_CFG_FILE}
                        SECURITY_PSL    ${SECURITY_PSL_FILE}
                        ${ENTITIES})

    # Create a top-level CMake target named sd-image which can be used
    # when building an SD card image to be started on the hardware platform.
    include (${KL_SDK_ROOT_PATH}/common/build-sd-image.cmake)
    build_sd_image (sd-image
                    KOS_IMAGE_TARGET kos-image)
else ()
    # Create a top-level CMake target named kos-qemu-image which
    # can be used when building a solution image to be started in QEMU.
    build_kos_qemu_image (kos-qemu-image
                          EINIT_ENTITY    EinitQemu
                          CONNECTIONS_CFG ${CONNECTIONS_CFG_FILE}
                          SECURITY_PSL    ${SECURITY_PSL_FILE}
                          QEMU_FLAGS      ${QEMU_FLAGS}
                          ${ENTITIES})
endif ()
