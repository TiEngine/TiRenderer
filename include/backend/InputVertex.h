#pragma once

#include <vector>
#include "BasicTypes.h"

namespace ti::backend {
class InputVertex {
public:
    struct Description {
        unsigned int verticesCount;
        unsigned int attributesByteSize;
        TransferDirection memoryType = TransferDirection::GPU_ONLY;

        Description(unsigned int verticesCount, unsigned int attributesByteSize,
            TransferDirection memoryType = TransferDirection::GPU_ONLY)
            : verticesCount(verticesCount), attributesByteSize(attributesByteSize)
            , memoryType(memoryType) {}
    };

    virtual void Upload(const std::vector<uint8_t>& data, bool sync = true) = 0;

protected:
    InputVertex() = default;
    virtual ~InputVertex() = default;
};
}
