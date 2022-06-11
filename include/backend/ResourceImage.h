#pragma once

#include "BasicTypes.h"

namespace ti::backend {

class ResourceImage {
public:
    struct Description {
        BasicFormat format;
        MSAA msaa;
        uint32_t width;  // 1st dimension
        uint32_t height; // 2nd dimension
        uint8_t arrays;  // 3rd dimension
        uint8_t mips;    // mipmap
        ImageType usage;
        ImageDimension dimension;
        ClearValue clearValue;
        TransferDirection memoryType;

        Description(
            BasicFormat format,
            uint32_t width,
            uint32_t height,
            uint8_t arrays = 1,
            uint8_t mips = 1,
            ClearValue clearValue = {},
            ImageType usage = ImageType::ShaderResource,
            ImageDimension dimension = ImageDimension::Dimension2D,
            TransferDirection memoryType = TransferDirection::GPU_ONLY,
            MSAA msaa = MSAA::MSAAx1)
            : format(format)
            , msaa(msaa)
            , width(width)
            , height(height)
            , arrays(arrays)
            , mips(mips)
            , usage(usage)
            , dimension(dimension)
            , clearValue(clearValue)
            , memoryType(memoryType)
        {}
    };

    virtual void* Map(unsigned int msaaLayer = 0) = 0;
    virtual void Unmap(unsigned int msaaLayer = 0) = 0;

protected:
    ResourceImage() = default;
    virtual ~ResourceImage() = default;
};

}
