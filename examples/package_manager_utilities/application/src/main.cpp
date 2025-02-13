/*
 * © 2025 AO Kaspersky Lab
 * Licensed under the MIT License
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cassert>
#include <unistd.h>

#include <application/Application.edl.h>

constexpr char Tag[]  = "[Application]";
constexpr char File[] = "/" APP_INSTALL_DIR "/" PACKAGE_ID "/res/text.txt";

int main([[maybe_unused]] int argc, [[maybe_unused]] const char *argv[])
{
    static_assert(
        (sizeof(PACKAGE_ID) > 1)
        && "PACKAGE_ID (ID of the installed KPA package) is empty! Set it in the CMakeLists.txt root file.");
    static_assert(
        (sizeof(APP_INSTALL_DIR) > 1)
        && "APP_INSTALL_DIR (path where KPA packages are installed) is empty! Set it in the CMakeLists.txt root file.");

    std::cerr << Tag << ": " << "Application started" << std::endl;

    std::cerr << Tag << ": " << "Open resource file " << File << std::endl;

    std::ifstream content{File};
    std::string line;

    std::cerr << Tag << ": " << "Content:" << std::endl;
    while(std::getline(content, line))
    {
        std::cerr << Tag << ": " << line << std::endl;
    }

    std::cerr << Tag << ": " << "Application finished" << std::endl;

    return EXIT_SUCCESS;
}
