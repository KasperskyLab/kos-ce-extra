# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

if (NOT DEFINED PACKAGE_ID OR ${PACKAGE_ID} STREQUAL "")
    message (FATAL_ERROR "PACKAGE_ID (ID of the installed KPA package) is empty! Set it in the CMakeLists.txt root file.")
endif ()
if (NOT DEFINED APP_INSTALL_DIR OR ${APP_INSTALL_DIR} STREQUAL "")
    message (FATAL_ERROR "APP_INSTALL_DIR (path where KPA packages are installed) is empty! Set it in the CMakeLists.txt root file.")
endif ()

# Set variable values for executable file names of CAS tools used for managing KPA packages.
set (CAS_PACK_CMD    ${KL_SDK_ROOT_PATH}/toolchain/bin/cas-pack)
set (CAS_INSPECT_CMD ${KL_SDK_ROOT_PATH}/toolchain/bin/cas-inspect)
set (CAS_PM_CMD      ${KL_SDK_ROOT_PATH}/toolchain/bin/cas-pm)

# Set variable values for the KPA package directories, name and extension.
set (SOURCE_PKG_DIR ${RESOURCES}/package)
set (PKG_DIR        ${CMAKE_CURRENT_BINARY_DIR}/package)
set (PKG_NAME       "Application")
set (PKG_EXT        "kpa")

# Get the SDK name and version to fill a KPA package manifest.
string (REGEX REPLACE "^.*/" "" SDK_BASENAME ${KL_SDK_ROOT_PATH})
string (REGEX MATCH "[0-9]+(\\.[0-9]+)*" SDK_VERSION ${SDK_BASENAME})
string (REPLACE "-${SDK_VERSION}" "" SDK_NAME ${SDK_BASENAME})

# Get the SDK architecture to fill the KPA package manifest.
set (ARCH ${CMAKE_SYSTEM_PROCESSOR})

# Get the Application program EIID to fill the KPA package manifest.
file (STRINGS ${NK_RESOURCES}/application/Application.edl EIID_LINE REGEX "task class +[a-zA-Z0-9]+(\\.[a-zA-Z0-9]+)*")
string (REGEX REPLACE "task class" "" APPLICATION_EIID ${EIID_LINE})
string (STRIP ${APPLICATION_EIID} APPLICATION_EIID)

set (APP_DATA_FILE "/${APP_INSTALL_DIR}/${PACKAGE_ID}/v${PACKAGE_VERSION}/res/text.txt")

# Set the variable values for the files needed for packing and installing of the KPA package.
set (SOURCE_KPA_FILES
     ${SOURCE_PKG_DIR}/manifest.json
     ${CMAKE_BINARY_DIR}/application/Application
     ${SOURCE_PKG_DIR}/files/text.txt
     ${SOURCE_PKG_DIR}/files/pkg-locale)
set (KPA_FILES
     ${PKG_DIR}/manifest.json
     ${PKG_DIR}/files/Application
     ${PKG_DIR}/files/text.txt
     ${PKG_DIR}/files/pkg-locale)
set (INSTALLED_KPA_FILES
     ${ROOTFS_DIR}/${APP_INSTALL_DIR}/${PACKAGE_ID}/bin/Application
     ${ROOTFS_DIR}${APP_DATA_FILE}
     ${ROOTFS_DIR}/${APP_INSTALL_DIR}/${PACKAGE_ID}/manifestLocale/pkg-locale
     ${ROOTFS_DIR}${PKG_DB_PATH})

# Copy the source files of the KPA package to one build directory.
add_custom_command (OUTPUT  ${KPA_FILES}
                    DEPENDS ${SOURCE_KPA_FILES}
                    COMMAND ${CMAKE_COMMAND} -E make_directory ${PKG_DIR}/files
                    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SOURCE_PKG_DIR}/files ${PKG_DIR}/files
                    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_PKG_DIR}/manifest.json ${PKG_DIR}
                    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/application/Application ${PKG_DIR}/files)

# Substitute macros in the KPA package manifest with real values.
# manifest.json file will be updated if the source file was updated.
add_custom_target (prepare_kpa_package_manifest
                   DEPENDS ${PKG_DIR}/manifest.json
                   COMMAND sed
                           -e s/__PACKAGE_ID__/${PACKAGE_ID}/
                           -e s/__PACKAGE_VERSION__/${PACKAGE_VERSION}/
                           -e s!__APP_DATA_FILE__!${APP_DATA_FILE}!
                           -e s/__SDK_NAME__/${SDK_NAME}/
                           -e s/__SDK_VERSION__/${SDK_VERSION}/
                           -e s/__ARCH__/${ARCH}/
                           -e s/__APPLICATION_EIID__/${APPLICATION_EIID}/
                           -e s/__APPLICATION_HASH__/`sha256sum ${PKG_DIR}/files/Application | cut -f 1 --delimiter=' '`/
                           -e s/__RESOURCE_HASH__/`sha256sum ${PKG_DIR}/files/text.txt | cut -f 1 --delimiter=' '`/
                           -e s/__LOCALE_HASH__/`sha256sum ${PKG_DIR}/files/pkg-locale | cut -f 1 --delimiter=' '`/
                           ${PKG_DIR}/manifest.json > ${PKG_DIR}/manifest_new.json
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PKG_DIR}/manifest_new.json ${PKG_DIR}/manifest.json)

# Pack source files into the KPA package.
# Source files must correspond to the KPA package manifest.
add_custom_command (OUTPUT  ${PKG_DIR}/${PKG_NAME}.${PKG_EXT}
                    DEPENDS prepare_kpa_package_manifest ${KPA_FILES}
                    COMMAND ${CAS_PACK_CMD}
                            --verify
                            --manifest ${PKG_DIR}/manifest.json
                            --output   ${PKG_DIR}/${PKG_NAME}.${PKG_EXT}
                            ${PKG_DIR}/files/*)

# Output the KPA package info.
add_custom_target (read_kpa_package
                   DEPENDS ${PKG_DIR}/${PKG_NAME}.${PKG_EXT}
                   COMMAND ${CAS_INSPECT_CMD}
                           --input  ${PKG_DIR}/${PKG_NAME}.${PKG_EXT}
                           --verify
                           dump)

# Install the KPA package into ROOTFS directory.
add_custom_command (OUTPUT  ${INSTALLED_KPA_FILES}
                    DEPENDS ${PKG_DIR}/${PKG_NAME}.${PKG_EXT}
                            read_kpa_package
                            create_hdd_dir
                    COMMAND ${CAS_PM_CMD}
                            --pkgsdir   ${PKG_DIR}
                            --reinstall
                            -vvv
                            --dbpath    ${ROOTFS_DIR}${PKG_DB_PATH}
                            --appsdir   ${ROOTFS_DIR}/${APP_INSTALL_DIR}
                            --extension ${PKG_EXT}
                            ${PKG_NAME})

# Add install target for other projects.
add_custom_target (install_kpa_package
                   DEPENDS ${INSTALLED_KPA_FILES})
