# © 2026 AO Kaspersky Lab
# Licensed under the MIT License

# Include the CMake library named image, which contains the solution image build scripts.
include (platform/image)

set (SECURITY_PSL_FILE    "src/security.psl")
set (CONNECTIONS_CFG_FILE "src/init.yaml.in")
set (IMAGE_SRC            "${RESOURCES}/hdd")
set (IMAGE_FS             "${CMAKE_BINARY_DIR}/hdd")
set (IMAGE_PART1_FS       "${IMAGE_FS}/part1")
set (IMAGE_PART2_FS       "${IMAGE_FS}/part2")
set (LIBS_PATH            "${IMAGE_PART2_FS}/lib")
set (QEMU_DISK_IMAGE      sdcard0.img)
set (QEMU_DEPENDENCIES    ${QEMU_DISK_IMAGE})
set (QEMU_FLAGS           "-drive file=${QEMU_DISK_IMAGE},if=sd,format=raw")
set (GINGER_DEFINITIONS   "BOARD=${CMAKE_SYSTEM_BOARD}")

# Define the ENTITIES variable with a list of executable files of programs.
set (ENTITIES python::python3
              precompiled_vfs::VfsSdCardFs)

# Configure the BlobContainer program to depend on the VFS and expose the required environment
# variable when the platform supports dynamic linking.
if (PLATFORM_SUPPORTS_DYNAMIC_LINKING)
    set_target_properties (blob_container::entity PROPERTIES
                           DEPENDS_ON_ENTITY "precompiled_vfs::VfsSdCardFs"
                           EXTRA_ENV "    VFS_FILESYSTEM_BACKEND: client:kl.VfsSdCardFs")
endif ()

# Set environment variables and arguments that will be passed to the Python process.
set (PYTHON_ENV "\
    VFS_FILESYSTEM_BACKEND: client:kl.VfsSdCardFs")

if (DEFINED PYTHON_EXTRA_ENV)
    string (APPEND PYTHON_ENV "\n${PYTHON_EXTRA_ENV}")
endif ()

set (PYTHON_ARGS "")

if (DEFINED PYTHON_EXTRA_ARGS)
    string (APPEND PYTHON_ARGS "${PYTHON_EXTRA_ARGS}")
endif ()

# Declare the dependencies for the Python (VFS and BlobContainer).
set (PYTHON_DEPENDS_ON precompiled_vfs::VfsSdCardFs
                       blob_container::entity)

set_target_properties (python::python3 PROPERTIES
                       DEPENDS_ON_ENTITY "${PYTHON_DEPENDS_ON}"
                       EXTRA_ENV         ${PYTHON_ENV}
                       EXTRA_ARGS        ${PYTHON_ARGS})

# Determine the MMC device number based on the target board.
if (CMAKE_SYSTEM_BOARD STREQUAL "RADXA_ROCK_3A")
    set (MMC_DEV_NUM 1)
elseif (CMAKE_SYSTEM_BOARD STREQUAL "RPI4_BCM2711")
    set (MMC_DEV_NUM 0)
elseif (CMAKE_SYSTEM_BOARD STREQUAL "QEMU")
    set (MMC_DEV_NUM 0)
else ()
    message (FATAL_ERROR "Unsupported board: ${CMAKE_SYSTEM_BOARD}")
endif ()

# Configure the VfsSdCardFs program: set up its environment and mount
# arguments (boot partition, root filesystem, etc.).
set_target_properties (precompiled_vfs::VfsSdCardFs PROPERTIES
    EXTRA_ENV "
    VFS_FILESYSTEM_BACKEND: server:kl.VfsSdCardFs
    ROOTFS: mmc${MMC_DEV_NUM},1 / ext4 0"
    EXTRA_ARGS "
    - -l
    - nodev /tmp ramfs 0
    - -l
    - devfs /dev devfs 0
    - -l
    - mmc${MMC_DEV_NUM},0 /boot fat32 0")

# Gather all Python shared-library modules that need to be packaged.
file (GLOB python_SHARED_LIBS "${python_STDLIB}/lib-dynload/*")

# Add a target to remove duplicate copies of shared libraries that were copied
# into the disk image filesystem.
list (TRANSFORM python_SHARED_LIBS
      REPLACE "${python_STDLIB}/lib-dynload/" "${LIBS_PATH}/" OUTPUT_VARIABLE python_SHARED_LIBS_FOR_RM)
add_custom_target (rm_extra_copies
                   COMMAND ${CMAKE_COMMAND} -E rm -- ${python_SHARED_LIBS_FOR_RM})

# Create the filesystem layout for the disk image:
# - create the two partitions,
# - copy the static resources,
# - copy the Python standard library.
add_custom_target (disk_image_fs
                   COMMAND ${CMAKE_COMMAND} -E make_directory ${IMAGE_PART1_FS}
                   COMMAND ${CMAKE_COMMAND} -E make_directory ${IMAGE_PART2_FS}
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${IMAGE_SRC} ${IMAGE_PART2_FS}
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${python_STDLIB} ${LIBS_PATH}/${python_PYTHON_NAME})

if (NOT "${CMAKE_SYSTEM_BOARD}" STREQUAL "QEMU")
    # Create a top-level CMake target named kos-image which can be used
    # when building a solution image to be started on the hardware platform.
    build_kos_hw_image (kos-image
                        EINIT_ENTITY              EinitHw
                        CONNECTIONS_CFG           ${CONNECTIONS_CFG_FILE}
                        SECURITY_PSL              ${SECURITY_PSL_FILE}
                        GINGER_ENABLE             TRUE
                        GINGER_DEFINITIONS        ${GINGER_DEFINITIONS}
                        PACK_DEPS_COPY_ONLY       ON
                        PACK_DEPS_LIBS_PATH       ${LIBS_PATH}
                        PACK_DEPS_COPY_TARGET     copylibs_hw
                        PACK_DEPS_EXTRA_LIBRARIES ${python_SHARED_LIBS}
                        IMAGE_FILES               ${ENTITIES})

    add_dependencies (rm_extra_copies copylibs_hw)
    add_dependencies (kos-image disk_image_fs rm_extra_copies)

    # Create a top-level CMake target named sd-image which can be used
    # when building an SD card image to be started on the hardware platform.
    include (${KL_SDK_ROOT_PATH}/common/build-sd-image.cmake)
    build_sd_image (sd-image
                    KOS_IMAGE_TARGET      kos-image
                    PATH_PARTITION1_IMAGE ${IMAGE_PART1_FS}
                    DISK_SIZE             512
                    PARTITION_CMD         -p1 ${IMAGE_PART1_FS} -%1 33 -f1 fat32
                                          -p2 ${IMAGE_PART2_FS} -%2 67 -f2 ext4)
else ()
    # Add a target to create disk image for QEMU.
    add_custom_target (${QEMU_DISK_IMAGE}
                       DEPENDS copylibs_qemu disk_image_fs
                       COMMAND ${KL_SDK_ROOT_PATH}/common/prepare_hdd_img.sh
                               -s 512
                               -p1 ${IMAGE_PART1_FS} -%1 33 -f1 fat32
                               -p2 ${IMAGE_PART2_FS} -%2 67 -f2 ext4
                               -img ${QEMU_DISK_IMAGE})

    add_dependencies (rm_extra_copies copylibs_qemu)
    add_dependencies (${QEMU_DISK_IMAGE} disk_image_fs rm_extra_copies)

    # Create a top-level CMake target named kos-qemu-image which
    # can be used when building a solution image to be started in QEMU.
    build_kos_qemu_image (kos-qemu-image
                          EINIT_ENTITY              EinitQemu
                          CONNECTIONS_CFG           ${CONNECTIONS_CFG_FILE}
                          SECURITY_PSL              ${SECURITY_PSL_FILE}
                          QEMU_FLAGS                ${QEMU_FLAGS}
                          QEMU_DEPENDENCIES         ${QEMU_DEPENDENCIES}
                          GINGER_ENABLE             TRUE
                          GINGER_DEFINITIONS        ${GINGER_DEFINITIONS}
                          PACK_DEPS_COPY_ONLY       ON
                          PACK_DEPS_LIBS_PATH       ${LIBS_PATH}
                          PACK_DEPS_COPY_TARGET     copylibs_qemu
                          PACK_DEPS_EXTRA_LIBRARIES ${python_SHARED_LIBS}
                          IMAGE_FILES               ${ENTITIES})
endif ()
