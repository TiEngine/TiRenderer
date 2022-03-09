#pragma once

#include "BasicTypes.h"

namespace ti::backend {
class InputVertex {
public:
    struct Description {
        unsigned int verticesCount;
        unsigned int attributesByteSize;
        TransferDirection memoryType;

        Description(
            unsigned int verticesCount,
            unsigned int attributesByteSize,
            TransferDirection memoryType = TransferDirection::GPU_ONLY)
            : verticesCount(verticesCount)
            , attributesByteSize(attributesByteSize)
            , memoryType(memoryType)
        {}
    };

    virtual void* Map() = 0;
    virtual void Unmap() = 0;

protected:
    InputVertex() = default;
    virtual ~InputVertex() = default;
};
}
