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

    void BuildDescriptor(ResourceBuffer& resource) override;
    void BuildDescriptor(ResourceImage& resource) override;

private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    DX12DescriptorHeap& heap;
    unsigned int indexInHeap = 0;

    // Descriptor handle increment size, i.e.
    // the descriptor size in the descriptor heap.
    UINT mResIncrSize = 0; // Resource: CBV/SRV/UAV
    UINT mSamIncrSize = 0; // Image Sampler
    UINT mRtvIncrSize = 0; // Render Target
    UINT mDsvIncrSize = 0; // Depth Stencil

    Description description{ DescriptorType::ConstantBuffer };
    D3D12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor;
    D3D12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor;
};
}
