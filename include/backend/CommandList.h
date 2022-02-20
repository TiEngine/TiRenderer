#pragma once

#include "PipelineState.h"

namespace ti::backend {
class CommandList {
public:
    struct Description {
        Description() {}
    };

    virtual void Reset(const PipelineState& pipelineState) = 0;

    virtual void Flush() = 0;

protected:
    CommandList() = default;
    virtual ~CommandList() = default;
};
}
