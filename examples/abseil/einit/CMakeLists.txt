# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

# Include the CMake library named image
# containing the solution image build scripts.
include (platform/image)

# Set the values of the variables.
set (ENTITIES             Hello)
set (SECURITY_PSL_FILE    "src/security.psl")
set (CONNECTIONS_CFG_FILE "src/init.yaml.in")
set (GINGER_DEFINITIONS   "BOARD=${CMAKE_SYSTEM_BOARD}")
set (IMAGE_FS             "${CMAKE_BINARY_DIR}/hdd")
set (LIBS_PATH            "${IMAGE_FS}/lib")
set (QEMU_DISK_IMAGE      sdcard0.img)
set (QEMU_FLAGS           "-nic none")

if (PLATFORM_SUPPORTS_DYNAMIC_LINKING)
    list (APPEND GINGER_DEFINITIONS "USE_DYNLD")
    set_target_properties (blob_container::entity PROPERTIES
                           DEPENDS_ON_ENTITY precompiled_vfs::VfsSdCardFs
                           EXTRA_ENV         "    VFS_FILESYSTEM_BACKEND: client:kl.VfsSdCardFs")

    add_custom_target (${QEMU_DISK_IMAGE}
                       DEPENDS copylibs_qemu
                       COMMAND ${CMAKE_COMMAND} -E make_directory ${IMAGE_FS}
                       COMMAND ${KL_SDK_ROOT_PATH}/common/prepare_hdd_img.sh
                               -d ${IMAGE_FS} -s 64 -f fat32 -img ${QEMU_DISK_IMAGE})

    set (QEMU_FLAGS        "${QEMU_FLAGS} -drive file=${QEMU_DISK_IMAGE},if=sd,format=raw")
    set (QEMU_DEPENDENCIES ${QEMU_DISK_IMAGE})
endif ()

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

    add_dependencies (kos-image copylibs_hw)

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
                          QEMU_FLAGS            "${QEMU_FLAGS}"
                          CONNECTIONS_CFG       ${CONNECTIONS_CFG_FILE}
                          SECURITY_PSL          ${SECURITY_PSL_FILE}
                          GINGER_ENABLE         TRUE
                          GINGER_DEFINITIONS    ${GINGER_DEFINITIONS}
                          QEMU_DEPENDENCIES     ${QEMU_DEPENDENCIES}
                          PACK_DEPS_COPY_ONLY   ON
                          PACK_DEPS_LIBS_PATH   ${LIBS_PATH}
                          PACK_DEPS_COPY_TARGET copylibs_qemu
                          IMAGE_FILES           ${ENTITIES})

endif ()
