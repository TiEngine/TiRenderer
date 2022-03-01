#include "common/TString.hpp"
#include "DX12Context.h"
#include "DX12Common.h"

namespace ti::backend {
DX12Device::DX12Device(Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi) : dxgi(dxgi)
{
    EnumAdapters();
}

DX12Device::~DX12Device()
{
    Shutdown();
}

void DX12Device::Setup(Description description)
{
    TI_LOG_I(TAG, "Create DX12 device: %p", this);
    this->description = description;

    bool createHardwareDeviceSuccess = false;
    LogOutIfFailedI(D3D12CreateDevice(
        NULL, // use default adapter
        D3D_FEATURE_LEVEL_12_0,
        IID_PPV_ARGS(&device))
        , createHardwareDeviceSuccess);

    if (createHardwareDeviceSuccess) {
        TI_LOG_I(TAG, "Created DX12 hardware device with default adapter.");
    } else {
        TI_LOG_I(TAG, "Create DX12 hardware device failed, fallback to use soft warp device.");
        Microsoft::WRL::ComPtr<IDXGIAdapter> softWarpAdapter;
        LogIfFailedF(dxgi->EnumWarpAdapter(IID_PPV_ARGS(&softWarpAdapter)));
        LogIfFailedF(D3D12CreateDevice(softWarpAdapter.Get(),
            D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device)));
    }

    D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
    commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    LogIfFailedF(device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&queue)));
    LogIfFailedF(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator)));

    // This fence is used to synchronize between CPU Host and GPU Device.
    LogIfFailedF(device->CreateFence(currentFence, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
}

void DX12Device::Shutdown()
{
    TI_LOG_I(TAG, "Destroy DX12 device: %p", this);
    device.Reset();
    queue.Reset();
    allocator.Reset();

    currentFence = 0;
    fence.Reset();

    swapchains.resize(0);
    commandRecorders.resize(0);
}

Shader* DX12Device::CreateShader(Shader::Description description)
{
    return CreateInstance<Shader>(shaders, description);
}

bool DX12Device::DestroyShader(Shader* shader)
{
    return DestroyInstance(shaders, shader);
}

Swapchain* DX12Device::CreateSwapchain(Swapchain::Description description)
{
    return CreateInstance<Swapchain>(swapchains, description, *this);
}

bool DX12Device::DestroySwapchain(Swapchain* swapchain)
{
    return DestroyInstance(swapchains, swapchain);
}

CommandRecorder* DX12Device::CreateCommandRecorder(CommandRecorder::Description description)
{
    return CreateInstance<CommandRecorder>(commandRecorders, description, *this);
}

bool DX12Device::DestroyCommandRecorder(CommandRecorder* commandRecorder)
{
    return DestroyInstance(commandRecorders, commandRecorder);
}

InputVertexAttributes* DX12Device::CreateInputVertexAttributes(InputVertexAttributes::Description description)
{
    return CreateInstance<InputVertexAttributes>(inputVertexLayouts, description);
}

bool DX12Device::DestroyInputVertexAttributes(InputVertexAttributes* inputVertexAttributes)
{
    return DestroyInstance(inputVertexLayouts, inputVertexAttributes);
}

InputVertex* DX12Device::CreateInputVertex(InputVertex::Description description)
{
    return CreateInstance<InputVertex>(inputVertices, description, *this);
}

bool DX12Device::DestroyInputVertex(InputVertex* inputVertex)
{
    return DestroyInstance(inputVertices, inputVertex);
}

void DX12Device::WaitIdle()
{
    // Advance the fence value to mark commands up to this fence point.
    currentFence++;

    // Add an instruction to the command queue to set a new fence point.
    // Because we are on the GPU timeline, the new fence point won't be
    // set until the GPU finishes processing all the commands prior to
    // this Signal().
    LogIfFailedF(queue->Signal(fence.Get(), currentFence));

    // Wait until the GPU has completed commands up to this fence point.
    if (fence->GetCompletedValue() < currentFence) {
        HANDLE eventHandle = CreateEventEx(NULL, NULL, 0, EVENT_ALL_ACCESS);
        if (eventHandle != NULL) {
            // Fire event when GPU hits current fence.
            LogIfFailedF(fence->SetEventOnCompletion(currentFence, eventHandle));
            // Wait until the GPU hits current fence event is fired.
            WaitForSingleObject(eventHandle, INFINITE);
            CloseHandle(eventHandle);
        } else {
            TI_LOG_F(TAG, "Device wait idle failed, can not create event!");
        }
    }
}

Microsoft::WRL::ComPtr<IDXGIFactory4> DX12Device::DXGIFactory()
{
    return dxgi;
}

Microsoft::WRL::ComPtr<ID3D12Device> DX12Device::NativeDevice()
{
    return device;
}

Microsoft::WRL::ComPtr<ID3D12CommandQueue> DX12Device::CommandQueue(CommandType type)
{
    return queue;
}

Microsoft::WRL::ComPtr<ID3D12CommandAllocator> DX12Device::CommandAllocator(CommandType type)
{
    return allocator;
}

void DX12Device::ResetCommandAllocator()
{
    // Reuse the all memory associated with command recording.
    // It will call all CommandList->Reset in this command allocator and release all memory.
    // Only can be reset when the associated command lists have finished execution on the GPU.
    LogIfFailedF(allocator->Reset());
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
        TI_LOG_D(TAG, "* %d : %s", i, std::to_string(desc.Description).c_str());
        adapters.emplace_back(adapter);
    }

    TI_LOG_D(TAG, "Enum each adapter outputs...");
    for (size_t n = 0; n < adapters.size(); n++) {
        TI_LOG_D(TAG, "Adapter %d Outputs:", n);

        // Adaptor output: usually is a displayer(monitor).
        size_t outputsCount = 0;
        IDXGIOutput* output = nullptr;
        for (UINT i = 0; adapters[n]->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND; i++) {
            DXGI_OUTPUT_DESC desc;
            output->GetDesc(&desc);
            TI_LOG_D(TAG, "* %d : %s", i, std::to_string(desc.DeviceName).c_str());

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
                    TI_LOG_D(TAG, "    %s", text.c_str());
                }
            }

            outputsCount++;
            ReleaseCOM(output);
        }

        if (outputsCount == 0) {
            TI_LOG_D(TAG, "Adapter %d has no output.", n);
        }
    }

    for (auto adapter : adapters) {
        ReleaseCOM(adapter);
    }
}
}
