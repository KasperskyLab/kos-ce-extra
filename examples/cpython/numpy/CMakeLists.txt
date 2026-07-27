# © 2026 AO Kaspersky Lab
# Licensed under the MIT License

cmake_minimum_required (VERSION 3.29)

project (numpy)

# Initialize CMake library for the KasperskyOS SDK.
include (platform)
# Enable legacy dynamic linking (statically links libkos, libc, etc. to the loader)
# for compatibility with projects created using earlier versions of the SDK.
initialize_platform (USE_DYNAMIC_LEGACY)

# Set compiler flags.
project_header_default ("STANDARD_GNU_17:YES" "STRICT_WARNINGS:YES")

# Find the vfs package with the VFS component libraries.
find_package (vfs REQUIRED)
include_directories (${vfs_INCLUDE})

# Find the precompiled_vfs package with the VFS component executable files.
find_package (precompiled_vfs REQUIRED)
include_directories (${precompiled_vfs_INCLUDE})

# Locate the host Python 3 executable for cross‑compilation.
# Disable cross‑root search and system environment to avoid picking the target Python.
find_program (HOST_PYTHON3
    NAMES "python3" REQUIRED
    NO_CMAKE_FIND_ROOT_PATH
    NO_SYSTEM_ENVIRONMENT_PATH)

# Find the python package with the Python 3 standard library.
find_package (python REQUIRED COMPONENTS PYTHON3 STDLIB)

set (RESOURCES ${CMAKE_SOURCE_DIR}/resources)

# Specify the pip requirements file for host tools needed to set up cross‑compilation.
set (HOST_REQUIREMENTS    ${CMAKE_CURRENT_SOURCE_DIR}/build_requirements.txt)

# Set the directory where the cross‑virtual environment will be created.
set (PYTHON_CROSSVENV_DIR ${CMAKE_BINARY_DIR}/cross_venv)
# Define the command to activate the cross‑virtual environment (used later in sub‑builds).
set (CROSSVENV_COMMAND    "source ${PYTHON_CROSSVENV_DIR}/bin/activate")

# Specify the version of pip to be used.
set (REQUIRED_PIP "pip==26.1.2")
# Create a custom target that builds the cross‑virtual environment.
# It installs pip, installs host requirements, and runs crossenv.
add_custom_target (crossenv
    COMMAND ${HOST_PYTHON3} -m ensurepip
    COMMAND ${HOST_PYTHON3} -m pip install ${REQUIRED_PIP}
    COMMAND ${HOST_PYTHON3} -m pip install -r ${HOST_REQUIREMENTS}
    COMMAND ${HOST_PYTHON3} -m crossenv ${python_PYTHON3_EXECUTABLE} ${PYTHON_CROSSVENV_DIR}
    COMMAND bash -c "${CROSSVENV_COMMAND}                 \
                     && build-pip install ${REQUIRED_PIP} \
                     && pip install ${REQUIRED_PIP}"
    VERBATIM USES_TERMINAL)

# Configure the Python interpreter arguments and environment for the solution.
# Use ${RESOURCES}/hdd as the root of the solution's file system.
set (PYTHON_EXTRA_ARGS "\
    - /hello_numpy.py")
set (PYTHON_EXTRA_ENV  "")

add_subdirectory (numpy)
add_subdirectory (einit)
