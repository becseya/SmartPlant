#pragma once

#include <array>

template<typename T, typename... N>
auto make_array(N&&... args) -> std::array<T, sizeof...(args)>
{
    return { std::forward<N>(args)... };
}

template<typename T>
constexpr std::size_t array_size(const T& array)
{
    return sizeof(array) / sizeof(array[0]);
}
