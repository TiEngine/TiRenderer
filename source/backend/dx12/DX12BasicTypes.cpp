#include "DX12BasicTypes.h"
#include <unordered_map>

namespace ti::backend {

DXGI_FORMAT ConvertFormat(Format format)
{
    static const std::unordered_map<Format, DXGI_FORMAT> mapper = {
        { Format::R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM }
    };
    return mapper.at(format);
}

unsigned int ConvertMSAA(MSAA msaa)
{
    return static_cast<unsigned int>(msaa);
}

}
