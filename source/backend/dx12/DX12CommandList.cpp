#include "DX12CommandList.h"
#include "DX12Common.h"

namespace ti::backend {
DX12CommandList::DX12CommandList(
    Microsoft::WRL::ComPtr<ID3D12Device> device,
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator)
    : device(device), allocator(allocator)
{
}

DX12CommandList::~DX12CommandList()
{
    Shutdown();
}

void DX12CommandList::Setup(Description description)
{
    TI_LOG_I(TAG, "Create DX12 command list: %p", this);
    this->description = description;

    LogIfFailedF(device->CreateCommandList(0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        allocator.Get(), // Associated command allocator
        NULL,            // Initial PipelineStateObject
        IID_PPV_ARGS(&recorder)));

    // Start off in a closed state.
    // This is because the first time we refer to the command list
    // we may Reset it, and it needs to be closed before calling Reset.
    recorder->Close();
}

void DX12CommandList::Shutdown()
{
    TI_LOG_I(TAG, "Destroy DX12 command list: %p", this);
    description = {};
    recorder.Reset();
}
}
