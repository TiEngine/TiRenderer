#pragma once

#include "BasicTypes.h"

namespace ti::backend {

class ResourceBuffer {
public:
    struct Description {
        unsigned int bufferBytesSize;
        TransferDirection memoryType;

        Description(
            unsigned int bufferBytesSize,
            TransferDirection memoryType = TransferDirection::CPU_TO_GPU)
            : bufferBytesSize(bufferBytesSize)
            , memoryType(memoryType)
        {}
    };

    virtual void* Map() = 0;
    virtual void Unmap() = 0;

protected:
    ResourceBuffer() = default;
    virtual ~ResourceBuffer() = default;
};

class StructuredBuffer {
public:
    struct Description {
    };

protected:
    StructuredBuffer() = default;
    virtual ~StructuredBuffer() = default;
};

class ReadWriteBuffer {
public:
    struct Description {
    };

protected:
    ReadWriteBuffer() = default;
    virtual ~ReadWriteBuffer() = default;
};

}
