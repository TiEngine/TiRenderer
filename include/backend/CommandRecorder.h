#pragma once

#include "PipelineState.h"

namespace ti::backend {
class CommandRecorder {
public:
    struct Description {
    };

    virtual void Reset(const PipelineState& pipelineState) = 0;

    virtual void Flush() = 0;

protected:
    CommandRecorder() = default;
    virtual ~CommandRecorder() = default;
};
}
