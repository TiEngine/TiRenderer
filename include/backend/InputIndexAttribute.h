#pragma once

#include "BasicTypes.h"

namespace ti::backend {
class InputIndexAttribute {
public:
    struct Description {
        unsigned int reserved = 1;
    };

    struct Attribute {
        IndexFormat format;
        PrimitiveTopology topology;

        Attribute(
            IndexFormat format,
            PrimitiveTopology topology)
            : format(format)
            , topology(topology)
        {}
    };

    virtual void SetAttribute(Attribute attribute) = 0;

protected:
    InputIndexAttribute() = default;
    virtual ~InputIndexAttribute() = default;
};
}
