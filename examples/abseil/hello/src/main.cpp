/*
 * © 2025 AO Kaspersky Lab
 * Licensed under the MIT License
 */

#include <iostream>
#include <string>
#include <vector>
#include <absl/strings/str_join.h>

int main()
{
    std::vector<std::string> v = {"foo", "bar", "baz"};
    std::string              s = absl::StrJoin(v, "-");
    std::cerr << "Joined string: " << s << "\n";
    return EXIT_SUCCESS;
}
