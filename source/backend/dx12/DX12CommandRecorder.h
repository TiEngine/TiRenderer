#pragma once

#include "backend/CommandRecorder.h"
#include "DX12BackendHeaders.h"

namespace ti::backend {
class DX12CommandRecorder : public CommandRecorder {
public:
    explicit DX12CommandRecorder(
        Microsoft::WRL::ComPtr<ID3D12Device> device,
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue,
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator);
    ~DX12CommandRecorder() override;

    void Setup(Description description);
    void Shutdown();

    void Reset(const PipelineState* pipelineState = nullptr) override;

    void Submit() override;
    void Wait(std::function<void()> coroutine = {}) override;

private:
    Microsoft::WRL::ComPtr<ID3D12Device> device;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator;

    Description description;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> recorder;

    UINT64 currentFence = 0;
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;
};
}
