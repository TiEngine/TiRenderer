#pragma once

#include "backend/ResourceBuffer.h"
#include "DX12CommandRecorder.h"

namespace ti::backend {
class DX12Device;
class DX12ResourceBuffer : public ResourceBuffer {
public:
    explicit DX12ResourceBuffer(DX12Device& device);
    ~DX12ResourceBuffer() override;

    void Setup(Description description);
    void Shutdown();

    void Upload(const std::vector<uint8_t>& data, bool sync) override;

    Microsoft::WRL::ComPtr<ID3D12Resource> Buffer();
    Microsoft::WRL::ComPtr<ID3D12Resource> Uploader();
    Microsoft::WRL::ComPtr<ID3D12Resource> Downloader();

protected:
    unsigned int CalculateConstantBufferBytesSize(unsigned int input);
    void UploadGpuOnly(const std::vector<uint8_t>& data, bool forceSync);
    void UploadCpuToGpu(const std::vector<uint8_t>& data, bool forceSync);

private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    Description description{ 0u };
    unsigned int alignedBytesSize = 0;
    Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
    Microsoft::WRL::ComPtr<ID3D12Resource> uploader;
    Microsoft::WRL::ComPtr<ID3D12Resource> downloader;
    DX12CommandRecorder* transfer = nullptr;
};
}
