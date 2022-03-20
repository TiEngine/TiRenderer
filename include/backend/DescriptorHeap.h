#pragma once

#include "BasicTypes.h"

namespace ti::backend {
class DescriptorHeap {
public:
    struct Description {
        unsigned int capacity; // max descriptor count
        DescriptorType type;   // descriptor heap type

        // A draw call in a pass can only set up one heap at a time,
        // so reserve enough descriptor capacity as possible.
        Description(
            unsigned int capacity,
            DescriptorType heapType)
            : capacity(capacity)
            , type(heapType)
        {}
    };

protected:
    DescriptorHeap() = default;
    virtual ~DescriptorHeap() = default;
};
}
