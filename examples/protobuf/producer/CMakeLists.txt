# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

# Create a CMake target for generating a *.edl.cpp.h file for a defined EDL file using the nkppmeta
# compiler.
add_nk_edl (protobuf_example_producer_edl "${NK_RESOURCES}/protobuf_example/Producer.edl"
            NK_MODULE "protobuf_example"
            LANG "CXX")

# Bind the CMake target to the main CMake target for building the application.
bind_nk_targets (protobuf_example_producer_edl protobuf_example_consumer_idl)

add_executable (Producer src/main.cpp)

bind_nk_targets (Producer protobuf_example_producer_edl)
target_compile_definitions (Producer PRIVATE AppTag="Producer")
target_link_libraries (Producer PRIVATE
                       addressbook
                       logrr_cpp::logrr_cpp-static
                       vfs::client)

# Disable the default VFS entity that is automatically provided by vfs::client.
set_target_properties (Producer PROPERTIES vfs_entity::entity_REPLACEMENT "")
