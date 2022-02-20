#include "DX12CommandAllocator.h"
#include "DX12Common.h"

namespace ti::backend {
DX12CommandAllocator::DX12CommandAllocator(
    Microsoft::WRL::ComPtr<ID3D12Device> device,
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue)
    : device(device), queue(queue)
{
}

DX12CommandAllocator::~DX12CommandAllocator()
{
    Shutdown();
}

void DX12CommandAllocator::Setup(Description description)
{
    TI_LOG_I(TAG, "Create DX12 command allocator: %p", this);
    this->description = description;

    LogIfFailedF(device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator)));
}

void DX12CommandAllocator::Shutdown()
{
    TI_LOG_I(TAG, "Destroy DX12 command allocator: %p", this);
    allocator.Reset();

    commandLists.resize(0);
}

CommandList* DX12CommandAllocator::CreateCommandList(CommandList::Description description)
{
    return CreateInstance<CommandList>(commandLists, description, device, queue, allocator);
}

bool DX12CommandAllocator::DestroyCommandList(CommandList* commandList)
{
    return DestroyInstance(commandLists, commandList);
}
}
