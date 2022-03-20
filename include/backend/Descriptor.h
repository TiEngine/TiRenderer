#pragma once

#include "BasicTypes.h"

namespace ti::backend {
class Descriptor {
public:
    struct Description {
        DescriptorType type;
    };

    ///virtual void BuildDescriptor() = 0;

protected:
    Descriptor() = default;
    virtual ~Descriptor() = default;
};
}
