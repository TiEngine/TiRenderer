#pragma once

#include <wrl.h>
#include <d3d12.h>
#include "backend/Device.h"

namespace ti::backend {

class DX12Device : public Device {
public:
    explicit DX12Device(Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi);
    ~DX12Device() override;

protected:
    // TODO:  Support select a adapter by custom.
    //        Currently only the default adapter is used.
    // FIXME: Adapters should not be placed in the DX12Device,
    //        and dxgi should not be passed to DX12Device too
    //        in construct function.
    //        DX12Device should not have the variable of dxgi!
    //        Move it to DX12Context and add CreateAdapter
    //        function in DX12Context.
    void EnumAdapters();

    void GetDeviceDescriptorSize();

private:
    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    // Descriptor handle increment size, i.e.
    // the descriptor size in the descriptor heap.
    UINT descriptorSizeOfRtv = 0;
    UINT descriptorSizeOfDsv = 0;
    UINT descriptorSizeOfSampler = 0;
    UINT descriptorSizeOfCbvSrvUav = 0;
};

}
