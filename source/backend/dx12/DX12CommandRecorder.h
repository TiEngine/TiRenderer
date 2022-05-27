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

    void BeginRecord(PipelineState* const pipelineState) override;
    void EndRecord() override;

    void RcBarrier(InputVertex* const resource,
        ResourceState before, ResourceState after) override;
    void RcBarrier(InputIndex* const resource,
        ResourceState before, ResourceState after) override;
    void RcBarrier(ResourceBuffer* const resource,
        ResourceState before, ResourceState after) override;
    void RcBarrier(ResourceImage* const resource,
        ResourceState before, ResourceState after) override;
    void RcBarrier(Swapchain* const swapchain,
        ResourceState before, ResourceState after) override;

    void RcUpload(const void* const data, size_t size,
        InputVertex* const destination, InputVertex* const staging) override;
    void RcUpload(const void* const data, size_t size,
        InputIndex* const destination, InputIndex* const staging) override;
    void RcUpload(const void* const data, size_t size,
        ResourceBuffer* const destination, ResourceBuffer* const staging) override;
    void RcUpload(const void* const data, size_t size,
        ResourceImage* const destination, ResourceImage* const staging) override;

    void RcSetViewports(const std::vector<Viewport>& viewports) override;
    void RcSetScissors(const std::vector<Scissor>& scissors) override;

    void RcClearColorAttachment(Swapchain* const swapchain) override;
    void RcClearColorAttachment(ResourceImage* const attachment) override;
    void RcClearDepthStencilAttachment(Swapchain* const swapchain) override;
    void RcClearDepthStencilAttachment(ResourceImage* const attachment) override;

    void RcSetRenderAttachments(
        Swapchain* const swapchain,
        const std::vector<Descriptor*>& colorAttachments,
        const std::vector<Descriptor*>& depthStencilAttachments,
        bool descriptorsContinuous) override;

    void RcSetVertex(const std::vector<InputVertex*>& vertices,
        InputVertexAttributes* const attributes, unsigned int startSlot) override;
    void RcSetIndex(InputIndex* const index, InputIndexAttribute* const attribute) override;

    void RcSetDescriptorHeap(const std::vector<DescriptorHeap*>& heaps) override;

    void RcSetDescriptor(unsigned int index, ResourceBuffer* const resource) override;
    void RcSetDescriptor(unsigned int index, ResourceImage* const resource) override;

    void RcSetDescriptors(unsigned int index, // DescriptorTable
        const std::vector<Descriptor*>& descriptors) override;

    void RcSetPipelineLayout(PipelineLayout* const layout) override;

    void RcDraw(InputIndex* const index) override;

    void Submit() override;
    void Wait() override;

    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CommandList();

private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    Description description{ "" };
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> recorder;

    UINT64 currentFence = 0;
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;
};
}
