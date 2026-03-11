// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#pragma once

#include <exception>
#include <stdexcept>

#define MAKE_CUSTOM_ERROR(code) MAKE_RETCODE(RC_CUSTOMER_TRUE, RC_SPACE_GENERAL, 0, code, "")

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

constexpr Retcode AnimalNotSupported = MAKE_CUSTOM_ERROR(1);
constexpr Retcode FoodNotSuitable    = MAKE_CUSTOM_ERROR(2);

} // namespace code

namespace exception {

DEFINE_EXCEPTION(AnimalNotSupported, "Animal not supported");
DEFINE_EXCEPTION(FoodNotSuitable, "Food not suitable");

} // namespace exception
} // namespace err
