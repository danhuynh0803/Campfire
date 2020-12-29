#pragma once
#include <functional>

namespace Utilty
{
    //For function pointer equality comparsion
    template<typename T, typename... U>
    static size_t GetAddress(const std::function<T(U...)>& f);
    wchar_t* CharToWChar(const char* text);
    char* WCharToChar(const wchar_t* text);
}