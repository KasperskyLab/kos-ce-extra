# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

set (LOCAL_MODULE_NAME ${PROJECT_NAME})
set (TASK_NAME "Hello")
set (TASK_EDL_TARGET ${TASK_NAME}_edl)

# Generate an EDL file containing a description of the process class.
generate_edl_file (${TASK_NAME} PREFIX ${LOCAL_MODULE_NAME})
# Create a CMake target for generating an .edl.h file
# for one defined EDL file using the NK compiler.
nk_build_edl_files (${TASK_EDL_TARGET} NK_MODULE ${LOCAL_MODULE_NAME} EDL ${EDL_FILE})

add_executable (${TASK_NAME} src/main.cpp)
add_dependencies (${TASK_NAME} ${TASK_EDL_TARGET})
target_link_libraries (${TASK_NAME} PRIVATE absl::strings)
