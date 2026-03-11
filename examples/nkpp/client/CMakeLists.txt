# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

set (PROGRAM_NAME Client)
set (ENTITIES ${ENTITIES} ${PROGRAM_NAME} PARENT_SCOPE)

# Create a CMake target for generating a *.idl.cpp.h header file for a defined IDL file using the
# nkppmeta compiler.
add_nk_idl (ANIMAL_CLIENT_IDL "${NK_RESOURCES}/example/Animal.idl"
            NK_MODULE         "example"
            LANG              "CXX")

# Create a CMake target for generating a *.edl.cpp.h file for a defined EDL file using the nkppmeta
# compiler.
add_nk_edl (${PROGRAM_NAME}_EDL "${NK_RESOURCES}/example/Client.edl"
            NK_MODULE           "example"
            LANG                "CXX")

add_executable (${PROGRAM_NAME} "src/main.cpp")
add_dependencies (${PROGRAM_NAME} ${PROGRAM_NAME}_EDL ANIMAL_CLIENT_IDL)
# Bind the CMake target to the main CMake target for building the application.
bind_nk_targets (${PROGRAM_NAME} ${PROGRAM_NAME}_EDL ANIMAL_CLIENT_IDL)
