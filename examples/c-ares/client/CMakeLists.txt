# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

# Create a CMake target for generating a *.edl.cpp.h file for a defined EDL file using the nkppmeta
# compiler.
add_nk_edl (CLIENT_EDL "${RESOURCES_XDL}/Client.edl" NK_MODULE example LANG "CXX")

# Bind the CMake target to the main CMake target for building the application.
bind_nk_targets (CLIENT_EDL ${RESOLVE_IDLS})

add_executable (Client src/main.cpp)
bind_nk_targets (Client CLIENT_EDL)

target_compile_options (Client PRIVATE -fexceptions)
target_include_directories (Client PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries (Client PRIVATE logrr_cpp::logrr_cpp ${CMAKE_THREAD_LIBS_INIT})

# Disable the default VFS entity that is automatically provided by vfs::client.
set_target_properties (Client PROPERTIES vfs::entity_REPLACEMENT "")
