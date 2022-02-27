#pragma once

#include "BasicTypes.h"

namespace ti::backend {
class InputIndex {
public:
    struct Description {
        unsigned int indicesCount;
        unsigned int indexByteSize;
        TransferDirection memoryType = TransferDirection::GPU_ONLY;

        Description(unsigned int indicesCount, unsigned int indexByteSize,
            TransferDirection memoryType = TransferDirection::GPU_ONLY)
            : indicesCount(indicesCount), indexByteSize(indexByteSize)
            , memoryType(memoryType) {}
    };

protected:
    InputIndex() = default;
    virtual ~InputIndex() = default;
};
}
