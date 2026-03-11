# © 2026 AO Kaspersky Lab
# Licensed under the MIT License

# Include the CMake library named image, which contains the solution image build scripts.
include (platform/image)

set (SECURITY_PSL_FILE    "src/security.psl")
set (CONNECTIONS_CFG_FILE "src/init.yaml.in")
set (GINGER_DEFINITIONS   "BOARD=${CMAKE_SYSTEM_BOARD}")

set (ENTITIES Client
              precompiled_vfs::VfsSdCardFs)

set_target_properties (Client PROPERTIES
    EXTRA_CONNECTIONS "\
  - target: kl.VfsSdCardFs
    id: kl.VfsSdCardFs"
    EXTRA_ENV "\
    VFS_FILESYSTEM_BACKEND: client:kl.VfsSdCardFs")

if (NOT CMAKE_SYSTEM_BOARD STREQUAL "QEMU")
    # Create a top-level CMake target named kos-image which can be used when building a solution
    # image to be started on the hardware platform.
    build_kos_hw_image (kos-image
                        EINIT_ENTITY        EinitHw
                        CONNECTIONS_CFG     ${CONNECTIONS_CFG_FILE}
                        SECURITY_PSL        ${SECURITY_PSL_FILE}
                        GINGER_ENABLE       TRUE
                        GINGER_DEFINITIONS  ${GINGER_DEFINITIONS}
                        IMAGE_FILES         ${ENTITIES})

    include (${KL_SDK_ROOT_PATH}/common/build-sd-image.cmake)
    build_sd_image (sd-image
                    KOS_IMAGE_TARGET kos-image)
endif ()
