#include "DX12DescriptorHeap.h"
#include "DX12BasicTypes.h"
#include "DX12Common.h"
#include "DX12Device.h"

namespace ti::backend {
DX12DescriptorHeap::DX12DescriptorHeap(DX12Device& internal) : internal(internal)
{
    device = internal.NativeDevice();
}

DX12DescriptorHeap::~DX12DescriptorHeap()
{
    Shutdown();
}

void DX12DescriptorHeap::Setup(Description description)
{
    this->description = description;

    if (description.capacity == 0) {
        TI_LOG_RET_F(TAG, "Create descriptor heap failed, capacity is zero!");
    }

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
    heapDesc.NumDescriptors = description.capacity;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    heapDesc.NodeMask = 0;
    LogIfFailedF(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap)));
}

void DX12DescriptorHeap::Shutdown()
{
    description = { 0u, DescriptorType::ShaderResource };
    heap.Reset();
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DX12DescriptorHeap::Heap()
{
    return heap;
}
}
