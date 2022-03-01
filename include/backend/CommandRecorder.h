#pragma once

#include <functional>
#include "backend/BasicTypes.h"
#include "PipelineState.h"
#include "InputVertex.h"
#include "InputIndex.h"

namespace ti::backend {
class CommandRecorder {
public:
    struct Description {
        CommandType type = CommandType::Graphics;
    };

    virtual void BeginRecord(const PipelineState* pipelineState = nullptr) = 0;
    virtual void EndRecord() = 0;

    virtual void RcBarrier(InputVertex& vertex, ResourceState before, ResourceState after) = 0;

    virtual void RcUpload(InputVertex& vertex, const std::vector<uint8_t>& data) = 0;
    virtual void RcUpload(InputIndex& index, const std::vector<uint8_t>& data) = 0;

    virtual void Submit() = 0;
    virtual void Wait() = 0;

protected:
    CommandRecorder() = default;
    virtual ~CommandRecorder() = default;
};
}
