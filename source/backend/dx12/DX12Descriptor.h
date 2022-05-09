#pragma once

#include "backend/Descriptor.h"
#include "DX12BackendHeaders.h"
#include "DX12BaseObject.h"

namespace ti::backend {
class DX12Device;
class DX12DescriptorHeap;
class DX12Descriptor : public Descriptor
    , DX12Object<DX12Descriptor> {
public:
    explicit DX12Descriptor(DX12Device& device, DX12DescriptorHeap& heap, unsigned int index);
    ~DX12Descriptor() override;

    void Setup(Description description);
    void Shutdown();

    void BuildDescriptor(ResourceBuffer* resource) override;
    void BuildDescriptor(ResourceImage* resource) override;

private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    DX12DescriptorHeap& heap;
    unsigned int indexInHeap = 0;

    // Descriptor handle increment size, i.e.
    // the descriptor size in the descriptor heap.
    UINT mResourceDescriptorHandleIncrementSize = 0;         // CBV/SRV/UAV
    UINT mImageSamplerDescriptorHandleIncrementSize = 0;     // ImageSampler
    UINT mRenderTargetViewDescriptorHandleIncrementSize = 0; // RTV
    UINT mDepthStencilViewDescriptorHandleIncrementSize = 0; // DSV

    Description description{ DescriptorType::ConstantBuffer };
    D3D12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor; // Only the CPU descriptor handle needs to be held,
    D3D12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor; // the GPU descriptor handle is unused.
};
}
