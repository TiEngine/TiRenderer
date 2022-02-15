#include "DX12Context.h"
#include "DX12Common.h"

#if defined(DEBUG) || defined(_DEBUG)
#include <dxgidebug.h> // IDXGIInfoQueue
#endif

namespace ti::backend {
DX12Context::DX12Context()
{
    UINT dxgiFactoryFlags = 0;

    #if defined(DEBUG) || defined(_DEBUG)
    TI_LOG_I(TAG, "Enable DX12 debug layer.");
    Microsoft::WRL::ComPtr<ID3D12Debug> debugger;
    LogIfFailedD(D3D12GetDebugInterface(IID_PPV_ARGS(&debugger)));
    if (debugger != nullptr) {
        debugger->EnableDebugLayer();
    }
    Microsoft::WRL::ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue)))) {
        dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
        // DX12 does not support redirect debug layer message currently, see:
        // https://devblogs.microsoft.com/directx/d3d12-debug-layer-message-callback/
        // Sample code is:
        // ComPtr<IDXGIInfoQueue1> dxgiInfoQueue;
        // dxgiInfoQueue->RegisterMessageCallback(..);
    }
    #endif

    TI_LOG_I(TAG, "Create DX12 context (and DXGIFactory): %p", this);
    LogIfFailedF(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgi)));
}

DX12Context::~DX12Context()
{
    TI_LOG_I(TAG, "Destroy DX12 context: %p", this);
}

Device* DX12Context::CreateDevice(Device::Description description)
{
    return CreateInstance<Device>(devices, description, dxgi);
}

bool DX12Context::DestroyDevice(Device* device)
{
    return DestroyInstance(devices, device);
}
}
