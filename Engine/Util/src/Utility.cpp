#include "Util/Utility.h"

wchar_t* Utilty::CharToWChar(const char* text)
{
    size_t size = strlen(text) + 1;
    wchar_t* wchar = new wchar_t[size];
    mbstowcs(wchar, text, size);
    return wchar;
}

char* Utilty::WCharToChar(const wchar_t* text)
{
    size_t size = wcslen(text) + 1;
    char* c = new char[size];
    wcstombs(c, text, size);
    return c;
}

template<typename T, typename... U>
size_t Utilty::GetAddress(const std::function<T(U...)>& f)
{
    typedef T(functionType)(U...);
    functionType** fnPointer = f.template target<functionType*>();
    return (size_t)*fnPointer;
}