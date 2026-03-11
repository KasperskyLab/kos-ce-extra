// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#include "FileUtility.h"

#include <fstream>
#include <sstream>

namespace FileUtility {

std::string ReadFile(const std::string& filename)
{
    std::ifstream file;
    file.open(filename);

    if (!file.is_open())
    {
        std::stringstream ss;
        ss << "File not found: " << filename;

        throw std::runtime_error(ss.str());
    }

    std::stringstream ss;

    ss << file.rdbuf();

    file.close();

    return ss.str();
}

} // namespace FileUtility
