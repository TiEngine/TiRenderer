#pragma once

#include "BasicTypes.h"
#include "ResourceBuffer.h"
#include "ResourceImage.h"

namespace ti::backend {
class Descriptor {
public:
    struct Description {
        DescriptorType type;
    };

    virtual void BuildDescriptor(ResourceBuffer& resource) = 0;
    virtual void BuildDescriptor(ResourceImage& resource) = 0;

protected:
    Descriptor() = default;
    virtual ~Descriptor() = default;
};
}
