# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

project (server)

set (PROGRAM_NAME Server)
set (ENTITIES ${ENTITIES} ${PROGRAM_NAME} PARENT_SCOPE)

add_nk_idl (ANIMAL_SERVER_IDL "${RESOURCES}/idl/Animal.idl"
            NK_MODULE         "example"
            LANG              "CXX")

add_nk_cdl (CAT_CDL   "${RESOURCES}/cdl/Cat.cdl"
            NK_MODULE "example"
            LANG      "CXX")
add_dependencies (CAT_CDL ANIMAL_SERVER_IDL)
bind_nk_targets (CAT_CDL ANIMAL_SERVER_IDL)

add_nk_cdl (DOG_CDL   "${RESOURCES}/cdl/Dog.cdl"
            NK_MODULE "example"
            LANG      "CXX")
add_dependencies (DOG_CDL ANIMAL_SERVER_IDL)
bind_nk_targets (DOG_CDL ANIMAL_SERVER_IDL)

add_nk_cdl (CATDOG_CDL "${RESOURCES}/cdl/CatDog.cdl"
            NK_MODULE  "example"
            LANG       "CXX")
add_dependencies (CATDOG_CDL ANIMAL_SERVER_IDL)
bind_nk_targets (CATDOG_CDL ANIMAL_SERVER_IDL)

add_nk_cdl (SOMETHING_CDL "${RESOURCES}/cdl/Something.cdl"
            NK_MODULE     "example"
            LANG          "CXX")
add_dependencies (SOMETHING_CDL ANIMAL_SERVER_IDL)
bind_nk_targets (SOMETHING_CDL ANIMAL_SERVER_IDL)

add_nk_edl (${PROGRAM_NAME}_EDL "${RESOURCES}/edl/Server.edl"
            NK_MODULE           "example"
            LANG                "CXX")
add_dependencies (${PROGRAM_NAME}_EDL CAT_CDL DOG_CDL CATDOG_CDL SOMETHING_CDL)
bind_nk_targets (${PROGRAM_NAME}_EDL CAT_CDL CATDOG_CDL DOG_CDL SOMETHING_CDL)

add_executable (${PROGRAM_NAME} "src/main.cpp")
add_dependencies (${PROGRAM_NAME} ${PROGRAM_NAME}_EDL)
bind_nk_targets (${PROGRAM_NAME} ${PROGRAM_NAME}_EDL CAT_CDL CATDOG_CDL DOG_CDL SOMETHING_CDL)
