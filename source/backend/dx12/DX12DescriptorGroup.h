#pragma once

#include "backend/DescriptorGroup.h"
#include "DX12BackendHeaders.h"
#include "DX12BaseObject.h"

namespace ti::backend {
class DX12Device;
class DX12DescriptorGroup : public DescriptorGroup
    , DX12Object<DX12DescriptorGroup> {
public:
    explicit DX12DescriptorGroup(DX12Device& device);
    ~DX12DescriptorGroup() override;

    void Setup(Description description);
    void Shutdown();

    void AddDescriptor(DescriptorType type,
        unsigned int id, unsigned int space, ShaderStage visibility) override;
    void AddDescriptors(DescriptorType type, unsigned int beginId, unsigned int endId,
        unsigned int space, ShaderStage visibility) override;

private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    Description description{ 0u };
    unsigned int descriptorCounter = 0;
    // AddDescriptor will add descriptor placeholder to root parameter directly,
    // AddDescriptors will use the descriptor range and add range to root parameter.
    std::vector<CD3DX12_ROOT_PARAMETER> parameters;
    std::vector<std::unique_ptr<CD3DX12_DESCRIPTOR_RANGE>> descriptorRanges;
};
}
