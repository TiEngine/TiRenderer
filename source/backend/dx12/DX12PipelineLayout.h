#pragma once

#include "backend/PipelineLayout.h"
#include "DX12BackendHeaders.h"
#include "DX12BaseObject.h"

namespace ti::backend {
class DX12Device;
class DX12PipelineLayout : public PipelineLayout
    , DX12Object<DX12PipelineLayout> {
public:
    explicit DX12PipelineLayout(DX12Device& device);
    ~DX12PipelineLayout() override;

    void Setup(Description description);
    void Shutdown();

    void AddGroup(DescriptorGroup& group) override;
    void BuildLayout() override;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> Signature();

private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    Description description{ 0u };
    std::vector<CD3DX12_ROOT_PARAMETER> parameters;
    std::vector<D3D12_STATIC_SAMPLER_DESC> samplers;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> signature;
};
}
