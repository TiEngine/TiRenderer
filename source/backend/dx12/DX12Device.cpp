#include "common/TString.hpp"
#include "DX12Context.h"
#include "DX12Common.h"

namespace ti::backend {

DX12Device::DX12Device(Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi) : dxgi(dxgi)
{
    EnumAdapters();

    TI_LOG_I(TAG, "Create DX12 device: " + std::to_string(reinterpret_cast<uintptr_t>(this)));
    bool createHardwareDeviceSuccess = false;
    LogOutIfFailedI(D3D12CreateDevice(
        NULL, // use default adapter
        D3D_FEATURE_LEVEL_11_0,
        IID_PPV_ARGS(&device))
        , createHardwareDeviceSuccess);

    if (createHardwareDeviceSuccess) {
        TI_LOG_I(TAG, "Created DX12 hardware device with default adapter.");
    } else {
        TI_LOG_I(TAG, "Create DX12 hardware device failed, fallback to use soft warp device.");
        Microsoft::WRL::ComPtr<IDXGIAdapter> softWarpAdapter;
        LogIfFailedF(dxgi->EnumWarpAdapter(IID_PPV_ARGS(&softWarpAdapter)));
        LogIfFailedF(D3D12CreateDevice(softWarpAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)));
    }

    GetDeviceDescriptorSize();
    CreateDeviceCommandQueue();
}

DX12Device::~DX12Device()
{
    TI_LOG_I(TAG, "Destroy DX12 device: " + std::to_string(reinterpret_cast<uintptr_t>(this)));
}

Swapchain* DX12Device::CreateSwapchain(Swapchain::Description description)
{
    swapchains.emplace_back(std::make_unique<DX12Swapchain>(dxgi, commandQueue));
    swapchains.back()->Setup(description);
    return swapchains.back().get();
}

bool DX12Device::DestroySwapchain(Swapchain* swapchain)
{
    for (auto iter = swapchains.begin(); iter != swapchains.end(); iter++) {
        if (swapchain == iter->get()) {
            swapchains.erase(iter);
            return true;
        }
    }
    return false;
}

void DX12Device::EnumAdapters()
{
    TI_LOG_D(TAG, "Enum adapters...");
    std::vector<IDXGIAdapter*> adapters;

    TI_LOG_D(TAG, "Adapters:");
    IDXGIAdapter* adapter = nullptr;
    for (UINT i = 0; dxgi->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; i++) {
        DXGI_ADAPTER_DESC desc;
        adapter->GetDesc(&desc);
        TI_LOG_D(TAG, "* " + std::to_string(i) + " : " + std::to_string(desc.Description));
        adapters.emplace_back(adapter);
    }

    TI_LOG_D(TAG, "Enum each adapter outputs...");
    for (size_t n = 0; n < adapters.size(); n++) {
        TI_LOG_D(TAG, "Adapter " + std::to_string(n)  + " Outputs:");

        // Adaptor output: usually is a displayer(monitor).
        size_t outputsCount = 0;
        IDXGIOutput* output = nullptr;
        for (UINT i = 0; adapters[n]->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND; i++) {
            DXGI_OUTPUT_DESC desc;
            output->GetDesc(&desc);
            TI_LOG_D(TAG, "* " + std::to_string(i) + " : " + std::to_string(desc.DeviceName));

            TI_LOG_D(TAG, "  - OutputDisplayModes");
            {
                constexpr DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM; // default back buffer format
                constexpr UINT flags = 0;

                UINT count = 0;
                output->GetDisplayModeList(format, flags, &count, nullptr);

                std::vector<DXGI_MODE_DESC> modes(count);
                output->GetDisplayModeList(format, flags, &count, &modes[0]);

                for (const auto& mode : modes) {
                    UINT n = mode.RefreshRate.Numerator;
                    UINT d = mode.RefreshRate.Denominator;
                    std::string text =
                        "Width = " + std::to_string(mode.Width) + ", " +
                        "Height = " + std::to_string(mode.Height) + ", " +
                        "Refresh = " + std::to_string(n) + "/" + std::to_string(d) +
                        "=" + std::to_string(static_cast<float>(n) / static_cast<float>(d));
                    TI_LOG_D(TAG, "    " + text);
                }
            }

            outputsCount++;
            ReleaseCOM(output);
        }

        if (outputsCount == 0) {
            TI_LOG_D(TAG, "Adapter " + std::to_string(n) + " has no output.");
        }
    }

    for (auto adapter : adapters) {
        ReleaseCOM(adapter);
    }
}

void DX12Device::GetDeviceDescriptorSize()
{
    descriptorSizeOfRtv = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    descriptorSizeOfDsv = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    descriptorSizeOfSampler = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
    descriptorSizeOfCbvSrvUav = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void DX12Device::CreateDeviceCommandQueue()
{
    TI_LOG_I(TAG, "Create device command queue.");
    D3D12_COMMAND_QUEUE_DESC desc{};
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    LogIfFailedF(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue)));
}

}
