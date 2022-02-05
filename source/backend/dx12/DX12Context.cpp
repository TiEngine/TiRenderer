#include "DX12Context.h"
#include "DX12Common.h"

namespace ti::backend {

DX12Context::DX12Context()
{
    #if defined(DEBUG) || defined(_DEBUG)
    TI_LOG_I(TAG, "Enable DX12 debug layer.");
    Microsoft::WRL::ComPtr<ID3D12Debug> debugger;
    LogIfFailedD(D3D12GetDebugInterface(IID_PPV_ARGS(&debugger)));
    if (debugger != nullptr) {
        debugger->EnableDebugLayer();
    }
    #endif
    TI_LOG_I(TAG, "Create DX12 context (and DXGIFactory): " +
        std::to_string(reinterpret_cast<uintptr_t>(this)));
    LogIfFailedF(CreateDXGIFactory1(IID_PPV_ARGS(&dxgi)));
}

DX12Context::~DX12Context()
{
    TI_LOG_I(TAG, "Destroy DX12 context: " +
        std::to_string(reinterpret_cast<uintptr_t>(this)));
}

Device* DX12Context::CreateDevice()
{
    devices.emplace_back(std::make_unique<DX12Device>(dxgi));
    return devices.back().get();
}

bool DX12Context::DestroyDevice(Device* device)
{
    for (auto iter = devices.begin(); iter != devices.end(); iter++) {
        if (device == iter->get()) {
            devices.erase(iter);
            return true;
        }
    }
    return false;
}

}
