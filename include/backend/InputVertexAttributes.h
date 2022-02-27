#pragma once

#include <string>
#include "BasicTypes.h"

namespace ti::backend {
class InputVertexAttributes {
public:
    struct Description {
        unsigned int reservedAttributesCount = 8;
    };

    struct Attribute {
        Format format;
        std::string semantic;
        unsigned int location;
        unsigned int stride;
        unsigned int slot = 0;
        enum class SlotClass {
            PerVertexData,
            PerInstanceData
        } slotClass = SlotClass::PerVertexData;

        Attribute(
            Format format, std::string semantic, unsigned int location, unsigned int stride,
            unsigned int slot = 0, SlotClass slotClass = SlotClass::PerVertexData)
            : format(format), semantic(semantic), location(location), stride(stride)
            , slot(slot), slotClass(slotClass) {}
    };

    virtual void AddAttribute(Attribute attribute) = 0;

protected:
    InputVertexAttributes() = default;
    virtual ~InputVertexAttributes() = default;
};
}
