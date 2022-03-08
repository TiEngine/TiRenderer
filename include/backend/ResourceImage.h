#pragma once

#include "BasicTypes.h"

namespace ti::backend {
class ResourceImage {
public:
    struct Description {
        Format format;
        ImageType type;
        uint32_t width;
        uint32_t height;
        uint8_t arrays = 1;
        uint8_t mips = 1;
        uint8_t dimension = 2;
        ClearValue clearValue;
        TransferDirection memoryType = TransferDirection::GPU_ONLY;

        Description(Format format, ImageType type, uint32_t width, uint32_t height,
            uint8_t arrays = 1, uint8_t mips = 1, uint8_t dimension = 2, ClearValue clearValue = {},
            TransferDirection memoryType = TransferDirection::GPU_ONLY)
            : format(format), type(type), width(width), height(height)
            , arrays(arrays), mips(mips), dimension(dimension), clearValue(clearValue)
            , memoryType(memoryType) {}
    };

protected:
    ResourceImage() = default;
    virtual ~ResourceImage() = default;
};
}
