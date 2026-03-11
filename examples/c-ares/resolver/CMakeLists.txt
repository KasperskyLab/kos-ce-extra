# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

find_package (c-ares REQUIRED)

# Create a CMake target for generating a *.edl.cpp.h file for a defined EDL file using the nkppmeta
# compiler.
add_nk_edl (NAME_RESOLVER_EDL "${RESOURCES_XDL}/NameResolver.edl" NK_MODULE example LANG "CXX")

# Bind the CMake target to the main CMake target for building the application.
bind_nk_targets (NAME_RESOLVER_EDL ${RESOLVE_IDLS})

add_executable (NameResolver src/main.cpp src/resolver.cpp)
bind_nk_targets (NameResolver NAME_RESOLVER_EDL)
target_compile_options (NameResolver PRIVATE -fexceptions)
target_include_directories (NameResolver PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries (NameResolver
    PRIVATE
        vfs::client
        logrr_cpp::logrr_cpp
        ${CMAKE_THREAD_LIBS_INIT}
        $<IF:$<BOOL:${PLATFORM_SUPPORTS_DYNAMIC_LINKING}>,c-ares::cares_shared,c-ares::cares_static>
)

# Disable the default VFS entity that is automatically provided by vfs::client.
set_target_properties (NameResolver PROPERTIES vfs::entity_REPLACEMENT "")
