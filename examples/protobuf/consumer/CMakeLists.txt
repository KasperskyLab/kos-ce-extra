# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

# Create a CMake target for generating a *.idl.cpp.h header file for a defined IDL file using the
# nkppmeta compiler
add_nk_idl (protobuf_example_consumer_idl "${NK_RESOURCES}/protobuf_example/IConsumer.idl"
            NK_MODULE "protobuf_example"
            LANG "CXX")

# Create a CMake target for generating a *.edl.cpp.h file for a defined EDL file using the nkppmeta
# compiler.
add_nk_edl (protobuf_example_consumer_edl "${NK_RESOURCES}/protobuf_example/Consumer.edl"
            NK_MODULE "protobuf_example"
            LANG "CXX")

# Bind the CMake target to the main CMake target for building the application.
bind_nk_targets (protobuf_example_consumer_edl protobuf_example_consumer_idl)

add_executable (Consumer src/main.cpp)

bind_nk_targets (Consumer protobuf_example_consumer_edl)
target_compile_definitions (Consumer PRIVATE AppTag="Consumer")
target_link_libraries (Consumer PRIVATE
                       addressbook
                       logrr_cpp::logrr_cpp-static)
