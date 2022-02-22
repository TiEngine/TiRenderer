#pragma once

#include <functional>
#include "backend/BasicTypes.h"
#include "PipelineState.h"

namespace ti::backend {
class CommandRecorder {
public:
    struct Description {
        CommandType type = CommandType::Graphics;
    };

    virtual void Reset(const PipelineState* pipelineState = nullptr) = 0;

    virtual void BeginRecord() = 0;
    virtual void EndRecord() = 0;

    virtual void Submit() = 0;
    virtual void Wait(std::function<void()> coroutine = {}) = 0;

protected:
    CommandRecorder() = default;
    virtual ~CommandRecorder() = default;
};
}
