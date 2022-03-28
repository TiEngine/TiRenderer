#pragma once

#include "Descriptor.h"

namespace ti::backend {
class DescriptorTable {
public:
    struct Description {
        unsigned int descriptorCount;

        Description(
            unsigned int descriptorCount)
            : descriptorCount(descriptorCount)
        {}
    };

    virtual void AddDescriptor(Descriptor& descriptor) = 0;
    virtual void ClearDescriptors() = 0;

protected:
    DescriptorTable() = default;
    virtual ~DescriptorTable() = default;
};
}
