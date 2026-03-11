// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#pragma once

#include <string>

namespace CommandLineArg {

bool TryParse(const std::string& argName, const std::string& argText, bool& value);
bool TryParse(const std::string& argName, const std::string& argText, std::string& value);

} // namespace CommandLineArg

