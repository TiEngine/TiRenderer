#pragma once

#include <vector>
#include "BasicTypes.h"

namespace ti::backend {
class InputVertex {
public:
    struct Description {
        unsigned int verticesCount;
        unsigned int attributesByteSize;
        TransferDirection memoryType = TransferDirection::GPU_ONLY;

        Description(unsigned int verticesCount, unsigned int attributesByteSize,
            TransferDirection memoryType = TransferDirection::GPU_ONLY)
            : verticesCount(verticesCount), attributesByteSize(attributesByteSize)
            , memoryType(memoryType) {}
    };

    virtual void Upload(const std::vector<uint8_t>& data, bool sync = true) = 0;
    virtual void Readback(std::vector<uint8_t>& data) = 0;

    ResourceState GetState() const
    {
        return state;
    }

protected:
    InputVertex() = default;
    virtual ~InputVertex() = default;

    void SetState(ResourceState resourceState)
    {
        state = resourceState;
    }

private:
    ResourceState state = ResourceState::UNDEFINED;
};
}
