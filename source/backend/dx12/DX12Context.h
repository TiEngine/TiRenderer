#pragma once

#include <vector>
#include <memory>
#include "backend/BackendContext.h"
#include "DX12BackendHeaders.h"
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
