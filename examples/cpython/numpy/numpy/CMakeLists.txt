# © 2026 AO Kaspersky Lab
# Licensed under the MIT License

# Include the CMake ExternalProject module.
include (ExternalProject)

# Define package name and version.
set (PACKAGE_NAME      "numpy")
set (PACKAGE_VERSION   "2.2.6")
set (FULL_PACKAGE_NAME "${PACKAGE_NAME}-${PACKAGE_VERSION}")

# Specify the source and destination paths for the Meson cross‑compilation file.
set (MESON_CROSS_FILE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_C_COMPILER_TARGET}-cross-file.ini)
set (MESON_CROSS_FILE_DST ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_C_COMPILER_TARGET}-cross-file.ini)

# Set the path to the optional patch file.
set (PATCH_SRC ${CMAKE_CURRENT_SOURCE_DIR}/patches/${CMAKE_C_COMPILER_TARGET}-${FULL_PACKAGE_NAME}.patch)

# Generate the Meson cross‑file from the template.
configure_file (${MESON_CROSS_FILE_SRC} ${MESON_CROSS_FILE_DST} @ONLY)

# Define the external project that builds NumPy.
ExternalProject_Add (${FULL_PACKAGE_NAME}
    DOWNLOAD_COMMAND  bash -c "${HOST_PYTHON3} -m unearth\
                               --no-binary -d <SOURCE_DIR> ${PACKAGE_NAME}==${PACKAGE_VERSION}"
    PATCH_COMMAND     bash -c "[ ! -f \"${PATCH_SRC}\" ] || patch -d ${FULL_PACKAGE_NAME} -p1 -i ${PATCH_SRC}"
    CONFIGURE_COMMAND bash -c "${CROSSVENV_COMMAND}\
                               && build-pip install -r <SOURCE_DIR>/${FULL_PACKAGE_NAME}/requirements/build_requirements.txt\
                               && cp ${PYTHON_CROSSVENV_DIR}/build/bin/ninja ${PYTHON_CROSSVENV_DIR}/build/bin/cython ${PYTHON_CROSSVENV_DIR}/bin/"
    BUILD_COMMAND     bash -c "${CROSSVENV_COMMAND}\
                               && pip wheel --no-cache --no-build-isolation\
                               -Csetup-args=\"--cross-file=${MESON_CROSS_FILE_DST}\"\
                               <SOURCE_DIR>/${FULL_PACKAGE_NAME}"
    INSTALL_COMMAND   bash -c "${CROSSVENV_COMMAND}\
                               && pip install --prefix=${CMAKE_BINARY_DIR}/hdd/part2/ <BINARY_DIR>/${FULL_PACKAGE_NAME}*.whl"
    DEPENDS           crossenv)

# Create a convenient target named numpy that depends on the external project.
add_custom_target (numpy DEPENDS ${FULL_PACKAGE_NAME})
