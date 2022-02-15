#pragma once

#include "backend/CommandList.h"
#include "DX12BackendHeaders.h"

namespace ti::backend {
class DX12CommandList : public CommandList {
public:
    explicit DX12CommandList(
        Microsoft::WRL::ComPtr<ID3D12Device> device,
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator);
    ~DX12CommandList() override;

    void Setup(Description description);
    void Shutdown();

private:
    Microsoft::WRL::ComPtr<ID3D12Device> device;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator;

    Description description;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> recorder;
};
}
