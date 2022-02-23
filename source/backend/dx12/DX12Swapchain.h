#pragma once

#include <vector>
#include "backend/Swapchain.h"
#include "DX12BackendHeaders.h"

namespace ti::backend {
class DX12Device;
class DX12Swapchain : public Swapchain {
public:
    explicit DX12Swapchain(DX12Device& device);
    ~DX12Swapchain() override;

    void Setup(Description description);
    void Shutdown();

    void Resize(unsigned int width, unsigned int height) override;

    void Present() override;

private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue;

    // Descriptor handle increment size, i.e.
    // the descriptor size in the descriptor heap.
    UINT descriptorSizeOfRtv = 0;
    UINT descriptorSizeOfDsv = 0;

    Description description{ nullptr, 0u, 0u };
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;

    int currentBufferIndex = 0;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> renderTargetHandlesInHeap; // render target views
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> depthStencilHandlesInHeap; // depth stencil views
    std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> renderTargetBuffer; // render target
    std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> depthStencilBuffer; // depth stencil
};
}
