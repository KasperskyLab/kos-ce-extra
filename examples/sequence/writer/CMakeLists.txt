# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

# Create a CMake target for generating a *.edl.cpp.h file for a defined EDL file using the nkppmeta
# compiler.
add_nk_edl (WRITER_EDL "${NK_RESOURCES}/sequence_example/Writer.edl"
            NK_MODULE  "sequence_example"
            LANG       "CXX")

# Bind the CMake target to the main CMake target for building the application.
bind_nk_targets (WRITER_EDL SEQUENCE_LOGGING_IDL)

add_executable (Writer "src/writer.cpp")
target_include_directories (Writer PRIVATE "${RESOURCES}/include")

bind_nk_targets (Writer WRITER_EDL)
