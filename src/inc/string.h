#pragma once

#include <spdlog/fmt/bundled/ostream.h>

using namespace fmt::literals;

template <typename...>
using void_t = void;

template <typename T, typename = void>
struct Has_to_string : std::false_type {
};

template <typename T>
struct Has_to_string<
    T, void_t<std::enable_if_t<std::is_same<
           std::string, decltype(std::declval<T>().to_string())>::value>>>
    : std::true_type {
};

template <typename T, typename = std::enable_if_t<Has_to_string<T>::value>>
std::ostream& operator<<(std::ostream& os, const T& obj)
{
  os << obj.to_string();
  return os;
}
