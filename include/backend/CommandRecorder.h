#pragma once

#include <functional>
#include "backend/BasicTypes.h"
#include "PipelineState.h"
#include "InputVertex.h"
#include "InputIndex.h"
#include "ResourceBuffer.h"
#include "ResourceImage.h"

namespace ti::backend {
class CommandRecorder {
public:
    struct Description {
        CommandType type = CommandType::Graphics;
    };

    virtual void BeginRecord(const PipelineState* pipelineState = nullptr) = 0;
    virtual void EndRecord() = 0;

    virtual void RcBarrier(InputVertex& vertex, ResourceState before, ResourceState after) = 0;
    virtual void RcBarrier(InputIndex& index, ResourceState before, ResourceState after) = 0;
    virtual void RcBarrier(ResourceBuffer& buffer, ResourceState before, ResourceState after) = 0;

    virtual void RcUpload(InputVertex& vertex, const std::vector<uint8_t>& data) = 0;
    virtual void RcUpload(InputIndex& index, const std::vector<uint8_t>& data) = 0;
    virtual void RcUpload(ResourceBuffer& buffer, const std::vector<uint8_t>& data) = 0;

    virtual void RcSetViewports(const std::vector<Viewport>& viewports) = 0;
    virtual void RcSetScissors(const std::vector<Scissor>& scissors) = 0;

    virtual void RcClearColorAttachment(Swapchain& swapchain) = 0;
    virtual void RcClearColorAttachment(ResourceImage& attachment) = 0;
    virtual void RcClearDepthStencilAttachment(ResourceImage& attachment) = 0;

    virtual void RcSetRenderAttachments(
        const std::vector<Swapchain*>& swapchains,
        const std::vector<ResourceImage*>& colorAttachments,
        const std::vector<ResourceImage*>& depthStencilAttachments,
        bool descriptorsContinuous) = 0;

    virtual void RcSetVertex(const std::vector<InputVertex*>& vertices) = 0;
    virtual void RcSetIndex(InputIndex* index) = 0;

    virtual void Submit() = 0;
    virtual void Wait() = 0;

protected:
    CommandRecorder() = default;
    virtual ~CommandRecorder() = default;
};
}
