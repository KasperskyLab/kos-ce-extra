# © 2025 AO Kaspersky Lab
# Licensed under the MIT License

add_library (utility_lib
             CommandLineArg.h
             CommandLineArg.cc
             FileUtility.h
             FileUtility.cc)

target_include_directories (utility_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
