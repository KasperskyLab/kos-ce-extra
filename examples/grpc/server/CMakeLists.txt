# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

# Define SSL/TLS certificate and key locations.
set (SSL_CERTS
     "${SSL_CERTIFICATE_DIR}/ca.pem"
     "${SSL_CERTIFICATE_DIR}/server1.pem"
     "${SSL_CERTIFICATE_DIR}/server1.key")

add_executable (GreeterServer
                src/main.cc
                src/greeter_service_impl.h
                src/greeter_service_impl.cc)

target_link_libraries (GreeterServer helloworld_grpc_proto utility_lib)

if (KOS)
    # Create a CMake target for generating an .edl.h file for one defined EDL file using the NK
    # compiler.
    nk_build_edl_files (HelloWorld_server_edl_files NK_MODULE "helloworld" EDL "${RESOURCES}/xdl/helloworld/GreeterServer.edl")
    add_dependencies (GreeterServer HelloWorld_server_edl_files)
    target_link_libraries (GreeterServer vfs::client ${CMAKE_THREAD_LIBS_INIT})
    set (MAIN_TASK GreeterServer PARENT_SCOPE)
    set (SSL_CERTS ${SSL_CERTS}  PARENT_SCOPE)
else ()
    add_custom_command (TARGET GreeterServer POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy ${SSL_CERTS} ${CMAKE_CURRENT_BINARY_DIR})
endif ()
