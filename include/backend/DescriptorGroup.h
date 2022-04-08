#pragma once

#include "BasicTypes.h"

namespace ti::backend {
class DescriptorGroup {
public:
    struct Description {
        unsigned int descriptorCount;

        Description(
            unsigned int descriptorCount)
            : descriptorCount(descriptorCount)
        {}
    };

    virtual void AddDescriptor(DescriptorType type,
        unsigned int id, unsigned int space, ShaderStage visibility) = 0;
    virtual void AddDescriptors(DescriptorType type,
        unsigned int beginId, unsigned int endId, unsigned int space, ShaderStage visibility) = 0;

protected:
    DescriptorGroup() = default;
    virtual ~DescriptorGroup() = default;
};
}
