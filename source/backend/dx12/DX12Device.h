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

protected:
    // TODO:  Support select a adapter by custom.
    //        Currently only the default adapter is used.
    // FIXME: Adapters should not be placed in the DX12Device.
    //        Move it to DX12Context and add CreateAdapter function in DX12Context.
    void EnumAdapters();

    void GetDeviceDescriptorSize();  // called in the construct function
    void CreateDeviceCommandQueue(); // called in the construct function

private:
    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    // Descriptor handle increment size, i.e.
    // the descriptor size in the descriptor heap.
    UINT descriptorSizeOfRtv = 0;
    UINT descriptorSizeOfDsv = 0;
    UINT descriptorSizeOfSampler = 0;
    UINT descriptorSizeOfCbvSrvUav = 0;

    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;

    std::vector<DX12Swapchain> swapchains;
};

}
