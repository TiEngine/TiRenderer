#pragma once

#include <vector>
#include <memory>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include "backend/BackendContext.h"
#include "DX12Device.h"

namespace ti::backend {

class DX12Context : public BackendContext {
public:
    DX12Context();
    ~DX12Context() override;

    Device* CreateDevice() override;
    bool DestroyDevice(Device* device) override;

private:
    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi;
    std::vector<std::unique_ptr<DX12Device>> devices;
};

}
