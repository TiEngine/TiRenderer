#pragma once

#include "backend/DescriptorHeap.h"
#include "DX12BackendHeaders.h"
#include "DX12BaseObject.h"

namespace ti::backend {
class DX12Device;
class DX12DescriptorHeap : public DescriptorHeap
    , DX12Object<DX12DescriptorHeap> {
public:
    explicit DX12DescriptorHeap(DX12Device& device);
    ~DX12DescriptorHeap() override;

    void Setup(Description description);
    void Shutdown();

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Heap();

private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    Description description{ 0u, DescriptorType::ShaderResource };
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap;
};
}
