#pragma once

#include <vector>
#include "backend/BasicTypes.h"
#include "InputVertex.h"
#include "InputIndex.h"
#include "DescriptorHeap.h"
#include "PipelineLayout.h"
#include "PipelineState.h"
#include "Swapchain.h"

namespace ti::backend {
class CommandRecorder {
public:
    struct Description {
        CommandType type = CommandType::Graphics;
    };

    virtual void BeginRecord(PipelineState* const pipelineState = nullptr) = 0;
    virtual void EndRecord() = 0;

    virtual void RcBarrier(InputVertex* const resource,
        ResourceState before, ResourceState after) = 0;
    virtual void RcBarrier(InputIndex* const resource,
        ResourceState before, ResourceState after) = 0;
    virtual void RcBarrier(ResourceBuffer* const resource,
        ResourceState before, ResourceState after) = 0;
    virtual void RcBarrier(ResourceImage* const resource,
        ResourceState before, ResourceState after) = 0;
    virtual void RcBarrier(Swapchain* const swapchain,
        ResourceState before, ResourceState after) = 0;

    virtual void RcUpload(const void* const data, size_t size,
        InputVertex* const destination, InputVertex* const staging) = 0;
    virtual void RcUpload(const void* const data, size_t size,
        InputIndex* const destination, InputIndex* const staging) = 0;
    virtual void RcUpload(const void* const data, size_t size,
        ResourceBuffer* const destination, ResourceBuffer* const staging) = 0;
    virtual void RcUpload(const void* const data, size_t size,
        ResourceImage* const destination, ResourceImage* const staging) = 0;

    virtual void RcSetViewports(const std::vector<Viewport>& viewports) = 0;
    virtual void RcSetScissors(const std::vector<Scissor>& scissors) = 0;

    virtual void RcClearColorAttachment(Swapchain* const swapchain) = 0;
    virtual void RcClearColorAttachment(ResourceImage* const attachment) = 0;
    virtual void RcClearDepthStencilAttachment(Swapchain* const swapchain) = 0;
    virtual void RcClearDepthStencilAttachment(ResourceImage* const attachment) = 0;

    virtual void RcSetRenderAttachments(
        Swapchain* const swapchain,
        const std::vector<ResourceImage*>& colorAttachments,
        const std::vector<ResourceImage*>& depthStencilAttachments,
        bool descriptorsContinuous = false) = 0;

    virtual void RcSetVertex(const std::vector<InputVertex*>& vertices) = 0;
    virtual void RcSetIndex(InputIndex* const index) = 0;

    virtual void RcSetDescriptorHeap(const std::vector<DescriptorHeap*>& heaps) = 0;
    virtual void RcSetDescriptorGroups(const std::vector<DescriptorGroup*>& groups) = 0;

    virtual void RcSetPipelineLayout(PipelineLayout* const layout) = 0;

    virtual void RcDrawIndexed(InputIndex* const index) = 0;

    virtual void Submit() = 0;
    virtual void Wait() = 0;

protected:
    CommandRecorder() = default;
    virtual ~CommandRecorder() = default;
};
}
