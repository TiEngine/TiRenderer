#pragma once

#include <vector>
#include "backend/Swapchain.h"
#include "DX12BackendHeaders.h"

namespace ti::backend {

class DX12Swapchain : public Swapchain {
public:
    DX12Swapchain(
        Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi,
        Microsoft::WRL::ComPtr<ID3D12Device> device,
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue);
    ~DX12Swapchain() override;

    void Setup(Description description);
    void Shutdown();

    void Resize(unsigned int width, unsigned int height) override;

private:
    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi;
    Microsoft::WRL::ComPtr<ID3D12Device> device;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue;

    // Descriptor handle increment size, i.e.
    // the descriptor size in the descriptor heap.
    UINT descriptorSizeOfRtv = 0;
    UINT descriptorSizeOfDsv = 0;

    Description description;
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
