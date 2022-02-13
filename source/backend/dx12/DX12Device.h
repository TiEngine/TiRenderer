#pragma once

#include "backend/Device.h"
#include "DX12Swapchain.h"

namespace ti::backend {

class DX12Device : public Device {
public:
    explicit DX12Device(Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi);
    ~DX12Device() override;

    Swapchain* CreateSwapchain(Swapchain::Description description) override;
    bool DestroySwapchain(Swapchain* swapchain) override;

    void FlushAndWaitIdle() override;

protected:
    // TODO:  Support select a adapter by custom.
    //        Currently only the default adapter is used.
    // FIXME: Adapters should not be placed in the DX12Device.
    //        Move it to DX12Context and add CreateAdapter function in DX12Context.
    void EnumAdapters();

    void CreateDeviceCommandQueue();
    void DestroyDeviceCommandQueue();

private:
    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi;
    Microsoft::WRL::ComPtr<ID3D12Device> device;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue;

    UINT64 currentFence = 0;
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;

    std::vector<std::unique_ptr<DX12Swapchain>> swapchains;
};

}
