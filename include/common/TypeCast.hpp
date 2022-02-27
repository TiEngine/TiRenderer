#pragma once

#include <type_traits>

#ifdef TI_OPT_SAFE_DOWN_CAST
#define down_cast dynamic_cast
#else
#define down_cast static_cast
#endif

namespace ti::common {
template <typename T>
inline constexpr typename std::underlying_type<T>::type EnumCast(const T& value)
{
    return static_cast<std::underlying_type<T>::type>(value);
}
}
