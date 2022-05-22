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

    static void SetAligned(bool align);

    unsigned int GetBytesSize() const;
    unsigned int GetAlignedBytesSize() const;
    unsigned int GetAllocatedBytesSize() const;

    Microsoft::WRL::ComPtr<ID3D12Resource> Buffer();

protected:
    static unsigned int CalculateAlignedBytesSize(unsigned int input);

    static bool isAlignedBytesSize;

private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    Description description{ 0u };
    unsigned int alignedBytesSize = 0;
    unsigned int allocatedBytesSize = 0;
    Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
};
}
