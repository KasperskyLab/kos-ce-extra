// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <unistd.h>

#include <application/Application.edl.h>

constexpr char Tag[] = "[Application] ";

int main([[maybe_unused]] int argc, [[maybe_unused]] const char *argv[])
{
    std::cerr << Tag << "Application started" << std::endl;

    if (argc > 1)
    {
        std::cerr << Tag << "Open resource file " << argv[1] << std::endl;

        std::ifstream content{argv[1]};
        if (content.is_open())
        {
            std::cerr << Tag << "Content:" << std::endl;

            std::string line;
            while (std::getline(content, line))
            {
                std::cerr << Tag << line << std::endl;
            }
        }
        else
        {
            std::cerr << Tag << "Fails to open: " << strerror(errno) << std::endl;
        }
    }
    else
    {
        std::cerr << Tag << "Resource file is not specified" << std::endl;
    }

    std::cerr << Tag << "Application finished" << std::endl;

    return EXIT_SUCCESS;
}
