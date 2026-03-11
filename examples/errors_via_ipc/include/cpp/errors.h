// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#pragma once

#include <exception>
#include <stdexcept>

#define DEFINE_EXCEPTION(name, message)                                                            \
    class name: public std::exception                                                              \
    {                                                                                              \
    public:                                                                                        \
        const char* what() const noexcept override                                                 \
        {                                                                                          \
            return message;                                                                        \
        }                                                                                          \
    };

namespace err {
namespace code {

constexpr Retcode ServerBusy = rcBusy;

} // namespace code

namespace exception {

DEFINE_EXCEPTION(ServerBusy, "Server is busy");

} // namespace exception
} // namespace err
