#include "DX12Swapchain.h"
#include "DX12BasicTypes.h"
#include "DX12Common.h"

namespace ti::backend {

DX12Swapchain::DX12Swapchain(
    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi,
    Microsoft::WRL::ComPtr<ID3D12Device> device,
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue)
    : dxgi(dxgi), device(device), queue(queue)
{
}

DX12Swapchain::~DX12Swapchain()
{
    Finish();
}

void DX12Swapchain::Setup(Description description)
{
    TI_LOG_I(TAG, "Create DX12 swapchain: " +
        std::to_string(reinterpret_cast<uintptr_t>(this)));

    DXGI_SWAP_CHAIN_DESC desc{};
    desc.BufferDesc.Width = description.width;
    desc.BufferDesc.Height = description.height;
    desc.BufferDesc.RefreshRate.Numerator = description.refreshRate;
    desc.BufferDesc.RefreshRate.Denominator = 1;
    desc.BufferDesc.Format = ConvertFormat(description.format);
    desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = description.bufferCount;
    desc.OutputWindow = (HWND)(description.window);
    desc.Windowed = !description.fullScreen;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    desc.SampleDesc.Count = ConvertMSAA(description.msaa);
    desc.SampleDesc.Quality = 0;
    if (ConvertMSAA(description.msaa) > 1) {
        D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels{};
        msQualityLevels.Format = ConvertFormat(description.format);
        msQualityLevels.SampleCount = ConvertMSAA(description.msaa);
        msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
        msQualityLevels.NumQualityLevels = 0; // output variable
        bool checkFeatureSupportSuccess = false;
        LogOutIfFailedE(device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
            &msQualityLevels, sizeof(msQualityLevels)), checkFeatureSupportSuccess);
        if (checkFeatureSupportSuccess && (msQualityLevels.NumQualityLevels > 0)) {
            desc.SampleDesc.Quality = msQualityLevels.NumQualityLevels - 1;
        } else {
            desc.SampleDesc.Count = 1;
            TI_LOG_E(TAG, "Unexpected MSAA type or MSAA quality level, fallback to use MSAAx1.");
        }
    }

    // NB: Swapchain uses the queue to perform flush.
    LogIfFailedF(dxgi->CreateSwapChain(queue.Get(), &desc, swapchain.GetAddressOf()));
}

void DX12Swapchain::Finish()
{
    TI_LOG_I(TAG, "Destroy DX12 swapchain: " +
        std::to_string(reinterpret_cast<uintptr_t>(this)));
}

}
