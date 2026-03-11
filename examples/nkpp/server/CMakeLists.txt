# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

project (server)

set (PROGRAM_NAME Server)
set (ENTITIES ${ENTITIES} ${PROGRAM_NAME} PARENT_SCOPE)

# Create a CMake target for generating a *.idl.cpp.h header file for a defined IDL file using the
# nkppmeta compiler.
add_nk_idl (ANIMAL_SERVER_IDL "${NK_RESOURCES}/example/Animal.idl"
            NK_MODULE         "example"
            LANG              "CXX")

# Create a CMake target for generating a *.cdl.cpp.h file for a defined CDL file using the nkppmeta
# compiler.
add_nk_cdl (CAT_CDL   "${NK_RESOURCES}/example/Cat.cdl"
            NK_MODULE "example"
            LANG      "CXX")
add_dependencies (CAT_CDL ANIMAL_SERVER_IDL)
# Bind the CMake target to the main CMake target for building the application.
bind_nk_targets (CAT_CDL ANIMAL_SERVER_IDL)

add_nk_cdl (DOG_CDL   "${NK_RESOURCES}/example/Dog.cdl"
            NK_MODULE "example"
            LANG      "CXX")
add_dependencies (DOG_CDL ANIMAL_SERVER_IDL)
bind_nk_targets (DOG_CDL ANIMAL_SERVER_IDL)

add_nk_cdl (CATDOG_CDL "${NK_RESOURCES}/example/CatDog.cdl"
            NK_MODULE  "example"
            LANG       "CXX")
add_dependencies (CATDOG_CDL ANIMAL_SERVER_IDL)
bind_nk_targets (CATDOG_CDL ANIMAL_SERVER_IDL)

add_nk_cdl (SOMETHING_CDL "${NK_RESOURCES}/example/Something.cdl"
            NK_MODULE     "example"
            LANG          "CXX")
add_dependencies (SOMETHING_CDL ANIMAL_SERVER_IDL)
bind_nk_targets (SOMETHING_CDL ANIMAL_SERVER_IDL)

# Create a CMake target for generating a *.edl.cpp.h file for a defined EDL file using the nkppmeta
# compiler.
add_nk_edl (${PROGRAM_NAME}_EDL "${NK_RESOURCES}/example/Server.edl"
            NK_MODULE           "example"
            LANG                "CXX")
add_dependencies (${PROGRAM_NAME}_EDL CAT_CDL DOG_CDL CATDOG_CDL SOMETHING_CDL)
bind_nk_targets (${PROGRAM_NAME}_EDL CAT_CDL CATDOG_CDL DOG_CDL SOMETHING_CDL)

add_executable (${PROGRAM_NAME} "src/main.cpp")
add_dependencies (${PROGRAM_NAME} ${PROGRAM_NAME}_EDL)
bind_nk_targets (${PROGRAM_NAME} ${PROGRAM_NAME}_EDL CAT_CDL CATDOG_CDL DOG_CDL SOMETHING_CDL)
