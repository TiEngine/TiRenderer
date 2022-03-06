#pragma once

#include "backend/InputVertex.h"
#include "DX12CommandRecorder.h"

namespace ti::backend {
class DX12Device;
class DX12InputVertex : public InputVertex {
public:
    explicit DX12InputVertex(DX12Device& device);
    ~DX12InputVertex() override;

    void Setup(Description description);
    void Shutdown();

    void Upload(const std::vector<uint8_t>& data, bool sync) override;

    Microsoft::WRL::ComPtr<ID3D12Resource> Buffer();
    Microsoft::WRL::ComPtr<ID3D12Resource> Uploader();
    Microsoft::WRL::ComPtr<ID3D12Resource> Downloader();

protected:
    void UploadGpuOnly(const std::vector<uint8_t>& data, bool forceSync);
    void UploadCpuToGpu(const std::vector<uint8_t>& data, bool forceSync);

private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    Description description{ 0u, 0u };
    Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
    Microsoft::WRL::ComPtr<ID3D12Resource> uploader;
    Microsoft::WRL::ComPtr<ID3D12Resource> downloader;
    DX12CommandRecorder* transfer = nullptr;
};
}
