// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#pragma once

#include <string>
#include <array>
#include <variant>

template <class... Ts>
struct overloaded: Ts...
{
    using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

namespace food {

constexpr std::string_view Fish   = "fish";
constexpr std::string_view Meat   = "meat";
constexpr std::string_view Milk   = "milk";
constexpr std::string_view Orange = "orange";
constexpr std::string_view Apple  = "apple";
constexpr std::string_view Wood   = "wood";
constexpr std::string_view Stone  = "stone";

// clang-format off
constexpr std::array<std::string_view, 7> KnownFood = {
    Fish,
    Meat,
    Milk,
    Orange,
    Apple,
    Wood,
    Stone
};
// clang-format on

} // namespace food
