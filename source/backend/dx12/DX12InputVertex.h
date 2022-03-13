#pragma once

#include "backend/InputVertex.h"
#include "DX12BackendHeaders.h"
#include "DX12BaseObject.h"

namespace ti::backend {
class DX12Device;
class DX12InputVertex : public InputVertex
    , DX12Object<DX12InputVertex> {
public:
    explicit DX12InputVertex(DX12Device& device);
    ~DX12InputVertex() override;

    void Setup(Description description);
    void Shutdown();

    void* Map() override;
    void Unmap() override;

    Microsoft::WRL::ComPtr<ID3D12Resource> Buffer();

private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    Description description{ 0u, 0u };
    Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
};
}
