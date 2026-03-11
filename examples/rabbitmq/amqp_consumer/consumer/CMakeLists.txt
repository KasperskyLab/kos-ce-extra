# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

if (PLATFORM_SUPPORTS_DYNAMIC_LINKING)
    set (RABBITMQ_LIB rabbitmq::rabbitmq)
else ()
    set (RABBITMQ_LIB rabbitmq::rabbitmq-static)
endif ()

# Generate the *.edl.h file using the NK compiler for the Consumer program.
nk_build_edl_files (consumer_edl_files
                    NK_MODULE "amqp"
                    EDL       ${NK_RESOURCES}/amqp/Consumer.edl)

add_executable (Consumer
                src/main.cpp
                src/consumer.cpp
                ${UTILS}/utils.cpp)

target_compile_options (Consumer PUBLIC "-fexceptions")

add_dependencies (Consumer consumer_edl_files)

target_include_directories (Consumer PUBLIC ${UTILS})
target_link_libraries (Consumer 
                       logrr_cpp::logrr_cpp
                       vfs::client
                       ${RABBITMQ_LIB})

set_target_properties (Consumer PROPERTIES
                       vfs_entity::entity_REPLACEMENT "")
