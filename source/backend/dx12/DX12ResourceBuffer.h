#pragma once

#include "backend/ResourceBuffer.h"
#include "DX12BackendHeaders.h"
#include "DX12BaseObject.h"

namespace ti::backend {
class DX12Device;
class DX12ResourceBuffer : public ResourceBuffer
    , DX12Object<DX12ResourceBuffer> {
public:
    explicit DX12ResourceBuffer(DX12Device& device);
    ~DX12ResourceBuffer() override;

    void Setup(Description description);
    void Shutdown();

    void* Map() override;
    void Unmap() override;

    void SetAligned(bool align);
    Microsoft::WRL::ComPtr<ID3D12Resource> Buffer();

protected:
    unsigned int CalculateConstantBufferBytesSize(unsigned int input);

private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    Description description{ 0u };
    bool isAlignedBytesSize = false;
    unsigned int alignedBytesSize = 0;
    Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
};
}
