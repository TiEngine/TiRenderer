#pragma once

#include <vector>
#include "backend/BasicTypes.h"
#include "InputVertex.h"
#include "InputIndex.h"
#include "PipelineState.h"
#include "PipelineLayout.h"
#include "DescriptorHeap.h"
#include "Swapchain.h"

namespace ti::backend {
class CommandRecorder {
public:
    struct Description {
        CommandType type = CommandType::Graphics;
    };

    virtual void BeginRecord(PipelineState* pipelineState = nullptr) = 0;
    virtual void EndRecord() = 0;

    virtual void RcBarrier(InputVertex& resource,
        ResourceState before, ResourceState after) = 0;
    virtual void RcBarrier(InputIndex& resource,
        ResourceState before, ResourceState after) = 0;
    virtual void RcBarrier(ResourceBuffer& resource,
        ResourceState before, ResourceState after) = 0;
    virtual void RcBarrier(ResourceImage& resource,
        ResourceState before, ResourceState after) = 0;

    virtual void RcUpload(InputVertex& destination, InputVertex& staging,
        size_t size, const void* data) = 0;
    virtual void RcUpload(InputIndex& destination, InputIndex& staging,
        size_t size, const void* data) = 0;
    virtual void RcUpload(ResourceBuffer& destination, ResourceBuffer& staging,
        size_t size, const void* data) = 0;
    virtual void RcUpload(ResourceImage& destination, ResourceImage& staging,
        size_t size, const void* data) = 0;

    virtual void RcSetViewports(const std::vector<Viewport>& viewports) = 0;
    virtual void RcSetScissors(const std::vector<Scissor>& scissors) = 0;

    virtual void RcClearColorAttachment(Swapchain& swapchain) = 0;
    virtual void RcClearColorAttachment(ResourceImage& attachment) = 0;
    virtual void RcClearDepthStencilAttachment(Swapchain& swapchain) = 0;
    virtual void RcClearDepthStencilAttachment(ResourceImage& attachment) = 0;

    virtual void RcSetRenderAttachments(
        const std::vector<Swapchain*>& swapchains,
        const std::vector<ResourceImage*>& colorAttachments,
        const std::vector<ResourceImage*>& depthStencilAttachments,
        bool descriptorsContinuous = false) = 0;

    virtual void RcSetVertex(const std::vector<InputVertex*>& vertices) = 0;
    virtual void RcSetIndex(InputIndex* index) = 0;

    virtual void RcSetDescriptorHeap(const std::vector<DescriptorHeap*>& heaps) = 0;

    virtual void Submit() = 0;
    virtual void Wait() = 0;

protected:
    CommandRecorder() = default;
    virtual ~CommandRecorder() = default;
};
}
