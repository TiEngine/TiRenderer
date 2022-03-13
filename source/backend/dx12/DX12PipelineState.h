#pragma once

#include "backend/PipelineState.h"
#include "DX12BackendHeaders.h"
#include "DX12BaseObject.h"

namespace ti::backend {
class DX12Device;
class DX12PipelineState : public PipelineState
    , DX12Object<DX12PipelineState> {
public:
    explicit DX12PipelineState(DX12Device& device);
    ~DX12PipelineState() override;

    void Setup(Description description);
    void Shutdown();

private:
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    Description description;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
};
}
