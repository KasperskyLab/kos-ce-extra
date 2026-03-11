# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

# Set the SSL_CERTS variable to the full name of the CA's public key file,
# that will be used to verify certificate signatures.
set (SSL_CERTS "${SSL_CERTIFICATE_DIR}/ca.pem")

add_executable (GreeterClient
                src/main.cc
                src/greeter_client.h
                src/greeter_client.cc)

target_link_libraries (GreeterClient helloworld_grpc_proto utility_lib)

if (KOS)
    # Create a CMake target for generating an .edl.h file for one defined EDL file using the NK
    # compiler.
    nk_build_edl_files (HelloWorld_client_edl_files NK_MODULE "helloworld" EDL "${RESOURCES}/xdl/helloworld/GreeterClient.edl")

    add_dependencies (GreeterClient HelloWorld_client_edl_files)

    target_link_libraries (GreeterClient vfs::client)

    if (DEFAULT_ENDPOINT_ADDRESS)
        target_compile_definitions (GreeterClient PRIVATE DEFAULT_ENDPOINT_ADDRESS="${DEFAULT_ENDPOINT_ADDRESS}")
    endif()

    set (MAIN_TASK GreeterClient PARENT_SCOPE)
    set (SSL_CERTS ${SSL_CERTS}  PARENT_SCOPE)
else ()
    add_custom_command (TARGET GreeterClient POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy ${SSL_CERTS} ${CMAKE_CURRENT_BINARY_DIR})
endif ()
