#pragma once

#include <vector>
#include "BasicTypes.h"

namespace ti::backend {
class ResourceBuffer {
public:
    struct Description {
        unsigned int bufferBytesSize;
        TransferDirection memoryType = TransferDirection::CPU_TO_GPU;

        Description(unsigned int bufferBytesSize,
            TransferDirection memoryType = TransferDirection::CPU_TO_GPU)
            : bufferBytesSize(bufferBytesSize)
            , memoryType(memoryType) {}
    };

    virtual void Upload(const std::vector<uint8_t>& data, bool sync = false) = 0;

protected:
    ResourceBuffer() = default;
    virtual ~ResourceBuffer() = default;
};
}
