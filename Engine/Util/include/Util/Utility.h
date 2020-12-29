#pragma once
#include <functional>

namespace Utilty
{
    template<typename T, typename... U>
    static size_t GetAddress(std::function<T(U...)>);
}