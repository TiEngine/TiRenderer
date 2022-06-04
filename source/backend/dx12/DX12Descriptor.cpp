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
    mResourceDescriptorHandleIncrementSize =
        device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    mImageSamplerDescriptorHandleIncrementSize =
        device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
    mRenderTargetViewDescriptorHandleIncrementSize =
        device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    mDepthStencilViewDescriptorHandleIncrementSize =
        device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

DX12Descriptor::~DX12Descriptor()
{
    Shutdown();
}

void DX12Descriptor::Setup(Description description)
{
    this->description = description;

    descriptorHandleIncrementSize = 0;
    if (common::EnumCast(description.type) &
        common::EnumCast(DescriptorType::ShaderResource)) {
        descriptorHandleIncrementSize = mResourceDescriptorHandleIncrementSize;
    } else if (common::EnumCast(description.type) &
        common::EnumCast(DescriptorType::ImageSampler)) {
        descriptorHandleIncrementSize = mImageSamplerDescriptorHandleIncrementSize;
    } else if (common::EnumCast(description.type) &
        common::EnumCast(DescriptorType::ColorOutput)) {
        descriptorHandleIncrementSize = mRenderTargetViewDescriptorHandleIncrementSize;
    } else if (common::EnumCast(description.type) &
        common::EnumCast(DescriptorType::DepthStencil)) {
        descriptorHandleIncrementSize = mDepthStencilViewDescriptorHandleIncrementSize;
    } else {
        TI_LOG_RET_E(TAG, "The descriptor type is invalid!");
    }

    if (ConvertDescriptorHeap(description.type) != heap.GetHeapType()) {
        TI_LOG_RET_E(TAG, "The descriptor type is not match with the heap type!");
    }

    auto hCpu = CD3DX12_CPU_DESCRIPTOR_HANDLE(heap.Heap()->GetCPUDescriptorHandleForHeapStart());
    hCpu.Offset(indexInHeap, descriptorHandleIncrementSize);
    hCpuDescriptor = hCpu;

    auto hGpu = CD3DX12_GPU_DESCRIPTOR_HANDLE(heap.Heap()->GetGPUDescriptorHandleForHeapStart());
    hGpu.Offset(indexInHeap, descriptorHandleIncrementSize);
    hGpuDescriptor = hGpu;
}

void DX12Descriptor::Shutdown()
{
    description = { DescriptorType::ConstantBuffer };
    hCpuDescriptor = {};
    hGpuDescriptor = {};
    pResource = static_cast<void*>(nullptr);
}

void DX12Descriptor::BuildDescriptor(ResourceBuffer* resource)
{
    if (heap.GetHeapType() != D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) {
        TI_LOG_RET_E(TAG, "This descriptor heap and descriptor is not support buffer!");
    }

    auto dxResource = down_cast<DX12ResourceBuffer*>(resource);

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
    cbvDesc.BufferLocation = dxResource->Buffer()->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = dxResource->GetAllocatedBytesSize();

    device->CreateConstantBufferView(&cbvDesc, hCpuDescriptor);

    pResource = dxResource;
}

void DX12Descriptor::BuildDescriptor(ResourceImage* resource)
{
    auto dxResource = down_cast<DX12ResourceImage*>(resource);

    switch (heap.GetHeapType()) {
    case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
        device->CreateShaderResourceView(dxResource->Buffer().Get(), NULL, hCpuDescriptor);
        break;
    case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
        device->CreateRenderTargetView(dxResource->Buffer().Get(), NULL, hCpuDescriptor);
        break;
    case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
        device->CreateDepthStencilView(dxResource->Buffer().Get(), NULL, hCpuDescriptor);
        break;
    default:
        TI_LOG_RET_E(TAG, "This descriptor heap and descriptor is not support image!");
    }

    pResource = dxResource;
}

void DX12Descriptor::BuildDescriptor(ImageSampler* sampler)
{
    if (heap.GetHeapType() != D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER) {
        TI_LOG_RET_E(TAG, "This descriptor heap and descriptor is not support sampler!");
    }

    auto dxSampler = down_cast<DX12ImageSampler*>(sampler);

    device->CreateSampler(&dxSampler->NativeSamplerState(), hCpuDescriptor);

    pResource = dxSampler;
}

D3D12_CPU_DESCRIPTOR_HANDLE DX12Descriptor::AttachmentView() const
{
    return hCpuDescriptor;
}

D3D12_CPU_DESCRIPTOR_HANDLE DX12Descriptor::NativeCpuDescriptor() const
{
    return hCpuDescriptor;
}

D3D12_GPU_DESCRIPTOR_HANDLE DX12Descriptor::NativeGpuDescriptor() const
{
    return hGpuDescriptor;
}

bool DX12Descriptor::IsNativeDescriptorsContinuous(
    const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& descriptors)
{
    if (descriptors.size() > 0) {
        for (size_t n = 0; n < descriptors.size(); n++) {
            CD3DX12_CPU_DESCRIPTOR_HANDLE current(hCpuDescriptor);
            current.Offset(static_cast<INT>(n), descriptorHandleIncrementSize);
            if (current != descriptors[n]) {
                return false;
            }
        }
    }
    return true;
}

}
