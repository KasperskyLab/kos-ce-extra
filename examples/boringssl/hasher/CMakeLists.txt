# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

# Generate an EDL file containing a description of the process class.
generate_edl_file (Hasher PREFIX examples)
# Create a CMake target for generating an .edl.h file
# for one defined EDL file using the NK compiler.
nk_build_edl_files (hasher_edl_files NK_MODULE examples EDL "${EDL_FILE}")

add_executable (Hasher
                src/main.cpp)
add_dependencies (Hasher hasher_edl_files)
target_link_libraries (Hasher PRIVATE BoringSSL::Crypto BoringSSL::SSL)
