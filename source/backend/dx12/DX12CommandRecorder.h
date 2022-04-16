#pragma once

#include "backend/CommandRecorder.h"
#include "DX12BackendHeaders.h"
#include "DX12BaseObject.h"

namespace ti::backend {
class DX12Device;
class DX12CommandRecorder : public CommandRecorder
    , DX12Object<DX12CommandRecorder> {
public:
    explicit DX12CommandRecorder(DX12Device& device);
    ~DX12CommandRecorder() override;

    void Setup(Description description);
    void Shutdown();

    void BeginRecord(PipelineState* pipelineState) override;
    void EndRecord() override;

    void RcBarrier(InputVertex& resource,
        ResourceState before, ResourceState after) override;
    void RcBarrier(InputIndex& resource,
        ResourceState before, ResourceState after) override;
    void RcBarrier(ResourceBuffer& resource,
        ResourceState before, ResourceState after) override;
    void RcBarrier(ResourceImage& resource,
        ResourceState before, ResourceState after) override;

    void RcUpload(InputVertex& destination, InputVertex& staging,
                  size_t size, const void* data) override;
    void RcUpload(InputIndex& destination, InputIndex& staging,
                  size_t size, const void* data) override;
    void RcUpload(ResourceBuffer& destination, ResourceBuffer& staging,
                  size_t size, const void* data) override;
    void RcUpload(ResourceImage& destination, ResourceImage& staging,
                  size_t size, const void* data) override;

    void RcSetViewports(const std::vector<Viewport>& viewports) override;
    void RcSetScissors(const std::vector<Scissor>& scissors) override;

    void RcClearColorAttachment(Swapchain& swapchain) override;
    void RcClearColorAttachment(ResourceImage& attachment) override;
    void RcClearDepthStencilAttachment(Swapchain& swapchain) override;
    void RcClearDepthStencilAttachment(ResourceImage& attachment) override;

    void RcSetRenderAttachments(
        const std::vector<Swapchain*>& swapchains,
        const std::vector<ResourceImage*>& colorAttachments,
        const std::vector<ResourceImage*>& depthStencilAttachments,
        bool descriptorsContinuous) override;

    void RcSetVertex(const std::vector<InputVertex*>& vertices) override;
    void RcSetIndex(InputIndex* index) override;

    void RcSetDescriptorHeap(const std::vector<DescriptorHeap*>& heaps) override;

    void RcSetPipelineLayout(PipelineLayout& layout) override;

    void Submit() override;
    void Wait() override;

    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CommandList();

private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    Description description;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> recorder;

    UINT64 currentFence = 0;
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;
};
}
