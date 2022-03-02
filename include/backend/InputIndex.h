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

    virtual void Upload(const std::vector<uint8_t>& data, bool sync = true) = 0;
    virtual void Readback(std::vector<uint8_t>& data) = 0;

protected:
    InputIndex() = default;
    virtual ~InputIndex() = default;
};
}
