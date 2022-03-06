#pragma once

#include "backend/CommandRecorder.h"
#include "DX12BackendHeaders.h"

namespace ti::backend {
class DX12Device;
class DX12CommandRecorder : public CommandRecorder {
public:
    explicit DX12CommandRecorder(DX12Device& device);
    ~DX12CommandRecorder() override;

    void Setup(Description description);
    void Shutdown();

    void BeginRecord(const PipelineState* pipelineState = nullptr) override;
    void EndRecord() override;

    void RcBarrier(InputVertex& input, ResourceState before, ResourceState after) override;
    void RcBarrier(InputIndex& input, ResourceState before, ResourceState after) override;
    void RcBarrier(ResourceBuffer& buffer, ResourceState before, ResourceState after) override;

    void RcUpload(InputVertex& input, const std::vector<uint8_t>& data) override;
    void RcUpload(InputIndex& input, const std::vector<uint8_t>& data) override;
    void RcUpload(ResourceBuffer& buffer, const std::vector<uint8_t>& data) override;

    void RcSetViewports(const std::vector<Viewport>& viewports) override;
    void RcSetScissors(const std::vector<Scissor>& scissors) override;

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
