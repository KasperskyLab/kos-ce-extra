# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

# Create a CMake target for generating a *.idl.cpp.h header file for a defined IDL file using the
# nkppmeta compiler.
add_nk_idl (SEQUENCE_LOGGING_IDL "${NK_RESOURCES}/sequence_example/SequenceLogging.idl"
            NK_MODULE            "sequence_example"
            LANG                 "CXX")

# Create a CMake target for generating a *.edl.cpp.h file for a defined EDL file using the nkppmeta
# compiler.
add_nk_edl (READER_EDL "${NK_RESOURCES}/sequence_example/Reader.edl"
            NK_MODULE  "sequence_example"
            LANG       "CXX")

# Bind the CMake target to the main CMake target for building the application.
bind_nk_targets (READER_EDL SEQUENCE_LOGGING_IDL)

add_executable (Reader "src/reader.cpp")
target_include_directories (Reader PRIVATE "${RESOURCES}/include")

bind_nk_targets (Reader READER_EDL)
