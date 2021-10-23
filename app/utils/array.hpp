#pragma once

#include <array>

template<typename T, typename... N>
auto make_array(N&&... args) -> std::array<T, sizeof...(args)>
{
    return { std::forward<N>(args)... };
}
