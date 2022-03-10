#pragma once

#include "BasicTypes.h"

namespace ti::backend {
class ResourceImage {
public:
    struct Description {
        BasicFormat format;
        uint32_t width;
        uint32_t height;
        uint8_t arrays;
        uint8_t mips;
        uint8_t dimension;
        ImageType imageType;
        ClearValue clearValue;
        TransferDirection memoryType;

        Description(
            BasicFormat format,
            uint32_t width,
            uint32_t height,
            uint8_t arrays = 1,
            uint8_t mips = 1,
            uint8_t dimension = 2,
            ClearValue clearValue = {},
            ImageType imageType = ImageType::ShaderResource,
            TransferDirection memoryType = TransferDirection::GPU_ONLY)
            : format(format)
            , width(width)
            , height(height)
            , arrays(arrays)
            , mips(mips)
            , dimension(dimension)
            , imageType(imageType)
            , clearValue(clearValue)
            , memoryType(memoryType)
        {}
    };

    virtual void* Map() = 0;
    virtual void Unmap() = 0;

protected:
    ResourceImage() = default;
    virtual ~ResourceImage() = default;
};
}
