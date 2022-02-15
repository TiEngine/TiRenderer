#pragma once

#include "backend/BackendContext.h"
#include "DX12BackendHeaders.h"
#include "DX12Device.h"

namespace ti::backend {

class DX12Context : public BackendContext {
public:
    explicit DX12Context();
    ~DX12Context() override;

    Device* CreateDevice(Device::Description description) override;
    bool DestroyDevice(Device* device) override;

private:
    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi;
    std::vector<std::unique_ptr<DX12Device>> devices;
};

}
