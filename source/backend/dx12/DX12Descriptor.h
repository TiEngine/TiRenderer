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

private:
    DX12Device& internal;
    DX12DescriptorHeap& heap;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    Description description{ DescriptorType::ConstantBuffer };
    D3D12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor;
    D3D12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor;
};
}
