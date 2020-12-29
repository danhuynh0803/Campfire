#include "Util/Utility.h"

template<typename T, typename... U>
size_t Utilty::GetAddress(std::function<T(U...)> f)
{
    typedef T(functionType)(U...);
    functionType** fnPointer = f.template target<functionType*>();
    return (size_t)*fnPointer;
}