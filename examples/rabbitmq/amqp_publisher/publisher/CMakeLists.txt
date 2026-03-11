# © 2026 AO Kaspersky Lab
# Licensed under the MIT License

if (PLATFORM_SUPPORTS_DYNAMIC_LINKING)
    set (RABBITMQ_LIB rabbitmq::rabbitmq)
else()
    set (RABBITMQ_LIB rabbitmq::rabbitmq-static)
endif ()

# Generate the *.edl.h file using the NK compiler for the Publisher program.
nk_build_edl_files (publisher_edl_files
                    NK_MODULE "amqp"
                    EDL       ${NK_RESOURCES}/amqp/Publisher.edl)

add_executable (Publisher
                src/main.cpp
                src/publisher.cpp
                ${UTILS}/utils.cpp)

target_compile_options (Publisher PUBLIC "-fexceptions")

add_dependencies (Publisher publisher_edl_files)

target_include_directories (Publisher PUBLIC ${UTILS})
target_link_libraries (Publisher
                       logrr_cpp::logrr_cpp
                       vfs::client
                       ${RABBITMQ_LIB})

set_target_properties (Publisher PROPERTIES
                       vfs_entity::entity_REPLACEMENT "")
