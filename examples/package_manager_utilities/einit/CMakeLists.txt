# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

# Include the CMake library named image, which contains the solution image build
# scripts.
include (platform/image)

set (DISK_IMAGE "${CMAKE_BINARY_DIR}/hdd.img")

# Define the ENTITIES variable with a list of programs.
set (ENTITIES             ${ENTITIES}
                          precompiled_vfs::VfsSdCardFs)
set (EDL_TARGETS          Application)
set (CONNECTIONS_CFG_FILE "src/init.yaml.in")
set (SECURITY_PSL_FILE    "src/security.psl")
set (GINGER_DEFINITIONS   "BOARD=${CMAKE_SYSTEM_BOARD}")
set (QEMU_DEPENDENCIES    hdd_image)
set (QEMU_FLAGS           "-nographic -monitor none -nic none \
                          -drive file=${DISK_IMAGE},if=sd,format=raw")

add_custom_command (OUTPUT  ${ROOTFS_DIR}/lib
                    COMMAND ${CMAKE_COMMAND} -E make_directory ${ROOTFS_DIR}/lib)
add_custom_target (create_hdd_dir
                   DEPENDS ${ROOTFS_DIR}/lib)

# Create a top-level CMake target named hdd to generate an HDD with content.
# This target acts as a dependency for the copylibs_* targets. Also this target
# depends on both the create_hdd_dir target to ensure that the directory for the
# root filesystem libraries is created and the install_kpa_package target to
# ensure that the KPA package is installed before to execution.
add_custom_target (hdd
                   DEPENDS create_hdd_dir
                           install_kpa_package)

# Create a top-level CMake target named hdd_image to generate a disk image
# containing libraries. This target depends on the 'hdd' target and executes the
# script located at ${KL_SDK_ROOT_PATH}/common/prepare_hdd_img.sh with specified
# parameters to create the disk image in FAT32 format with a size of 64 MB.
add_custom_target (hdd_image
                   DEPENDS hdd
                   COMMAND ${KL_SDK_ROOT_PATH}/common/prepare_hdd_img.sh
                           -d   ${ROOTFS_DIR}
                           -img ${DISK_IMAGE}
                           -f   fat32
                           -s   64)

set_target_properties (blob_container::entity PROPERTIES
                       DEPENDS_ON_ENTITY precompiled_vfs::VfsSdCardFs
                       EXTRA_ENV         "    VFS_FILESYSTEM_BACKEND: client:kl.VfsSdCardFs")

if (NOT CMAKE_SYSTEM_BOARD STREQUAL "QEMU")
    # Create a top-level CMake target named kos-image which can be used when
    # building a solution image to be started on the hardware platform.
    build_kos_hw_image (kos-image
                        EINIT_ENTITY          EinitHw
                        EDL_TARGETS           ${EDL_TARGETS}
                        CONNECTIONS_CFG       ${CONNECTIONS_CFG_FILE}
                        SECURITY_PSL          ${SECURITY_PSL_FILE}
                        GINGER_ENABLE         TRUE
                        GINGER_DEFINITIONS    ${GINGER_DEFINITIONS}
                        PACK_DEPS_COPY_ONLY   ON
                        PACK_DEPS_LIBS_PATH   ${ROOTFS_DIR}/lib
                        PACK_DEPS_COPY_TARGET copylibs_hw
                        IMAGE_FILES           ${ENTITIES})
    add_dependencies (copylibs_hw hdd)
    add_dependencies (kos-image copylibs_hw)

    # Create a top-level CMake target named sd-image which can be used when building
    # an SD card image to be started on the hardware platform.
    include (${KL_SDK_ROOT_PATH}/common/build-sd-image.cmake)
    build_sd_image (sd-image
                    KOS_IMAGE_TARGET kos-image
                    IMAGE_FS         ${ROOTFS_DIR})
else ()
    # Create a top-level CMake target named kos-qemu-image which can be used when
    # building a solution image to be started on QEMU.
    build_kos_qemu_image (kos-qemu-image
                          EINIT_ENTITY          EinitQemu
                          EDL_TARGETS           ${EDL_TARGETS}
                          CONNECTIONS_CFG       ${CONNECTIONS_CFG_FILE}
                          SECURITY_PSL          ${SECURITY_PSL_FILE}
                          GINGER_ENABLE         TRUE
                          GINGER_DEFINITIONS    ${GINGER_DEFINITIONS}
                          QEMU_FLAGS            ${QEMU_FLAGS}
                          QEMU_DEPENDENCIES     ${QEMU_DEPENDENCIES}
                          PACK_DEPS_COPY_ONLY   ON
                          PACK_DEPS_LIBS_PATH   ${ROOTFS_DIR}/lib
                          PACK_DEPS_COPY_TARGET copylibs_qemu
                          IMAGE_FILES           ${ENTITIES})
    add_dependencies (copylibs_qemu hdd)
    add_dependencies (hdd_image copylibs_qemu)
endif ()
