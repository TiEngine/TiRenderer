#pragma once

namespace ti::backend {

enum class Format {
    R8G8B8A8_UNORM
};

enum class MSAA : unsigned int {
    MSAAx1 = 1,
    MSAAx4 = 4
};

}
