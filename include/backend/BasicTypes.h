#pragma once

namespace ti::backend {

enum class Format {
    R8G8B8A8_UNORM,
    D24_UNORM_S8_UINT
};

enum class MSAA : unsigned int {
    MSAAx1 = 1,
    MSAAx4 = 4
};

}
