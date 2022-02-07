#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include "backend/Swapchain.h"

namespace ti::backend {

class DX12Swapchain : public Swapchain {
public:
    DX12Swapchain(
        Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi,
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue);
    ~DX12Swapchain() override;
    void Setup(Description description) override;
    void Finish();

    //////

protected:


private:
    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;
};

}
