#include "DX12Descriptor.h"
#include "DX12BasicTypes.h"
#include "DX12Device.h"

namespace ti::backend {
DX12Descriptor::DX12Descriptor(DX12Device& internal,
    DX12DescriptorHeap& heap, unsigned int index)
    : internal(internal), heap(heap)
    , indexInHeap(index)
{
    device = internal.NativeDevice();
    mResIncrSize =device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    mSamIncrSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
    mRtvIncrSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    mDsvIncrSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

DX12Descriptor::~DX12Descriptor()
{
    Shutdown();
}

void DX12Descriptor::Setup(Description description)
{
    this->description = description;

    UINT descriptorHandleIncrementSize = 0;
    if (common::EnumCast(description.type) &
        common::EnumCast(DescriptorType::GenericBuffer)) {
        descriptorHandleIncrementSize = mResIncrSize;
    } else if (common::EnumCast(description.type) &
        common::EnumCast(DescriptorType::ImageSampler)) {
        descriptorHandleIncrementSize = mSamIncrSize;
    } else if (common::EnumCast(description.type) &
        common::EnumCast(DescriptorType::ColorOutput)) {
        descriptorHandleIncrementSize = mRtvIncrSize;
    } else if (common::EnumCast(description.type) &
        common::EnumCast(DescriptorType::DepthStencil)) {
        descriptorHandleIncrementSize = mDsvIncrSize;
    } else {
        TI_LOG_RET_E(TAG, "The descriptor type is invalid!");
    }

    if (ConvertDescriptorHeap(description.type) != heap.GetHeapType()) {
        TI_LOG_RET_E(TAG, "The descriptor type is not match with the heap type!");
    }

    auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(heap.Heap()->GetCPUDescriptorHandleForHeapStart());
    handle.Offset(indexInHeap, descriptorHandleIncrementSize);
}

void DX12Descriptor::Shutdown()
{
    description = { DescriptorType::ConstantBuffer };
    hCpuDescriptor = {};
    hGpuDescriptor = {};
}

void DX12Descriptor::BuildDescriptor(ResourceBuffer& resource)
{
    //TODO
}

void DX12Descriptor::BuildDescriptor(ResourceImage& resource)
{
    //TODO
}

}
