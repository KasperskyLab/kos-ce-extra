# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

project (Client)

set (PROGRAM_NAME Client)
set (ENTITIES ${ENTITIES} ${PROGRAM_NAME} PARENT_SCOPE)

add_nk_idl (ANIMAL_CLIENT_IDL "${RESOURCES}/idl/Animal.idl"
            NK_MODULE         "example"
            LANG              "CXX")

add_nk_edl (${PROGRAM_NAME}_EDL "${RESOURCES}/edl/Client.edl"
            NK_MODULE           "example"
            LANG                "CXX")

add_executable (${PROGRAM_NAME} "src/main.cpp")
add_dependencies (${PROGRAM_NAME} ${PROGRAM_NAME}_EDL ANIMAL_CLIENT_IDL)
bind_nk_targets (${PROGRAM_NAME} ${PROGRAM_NAME}_EDL ANIMAL_CLIENT_IDL)
