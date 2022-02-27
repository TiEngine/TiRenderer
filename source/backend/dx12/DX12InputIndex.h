#pragma once

#include "backend/InputIndex.h"
#include "DX12CommandRecorder.h"

namespace ti::backend {
class DX12Device;
class DX12InputIndex : public InputIndex {
public:
//    explicit DX12InputIndex(DX12Device& device);
//    ~DX12InputIndex() override;
//
//    void Setup(Description description);
//    void Shutdown();
//
//    void Upload(const std::vector<uint8_t>& data, bool sync = true) override;
//    void Readback(std::vector<uint8_t>& data) override;
//
//    void SetState(ResourceState state);
//
//    Microsoft::WRL::ComPtr<ID3D12Resource> Buffer();
//    Microsoft::WRL::ComPtr<ID3D12Resource> Uploader();
//    Microsoft::WRL::ComPtr<ID3D12Resource> Downloader();
//
//protected:
//    ResourceState SelectInitialResourceState(TransferDirection memoryType);
//
//    void UploadGpuOnly(const std::vector<uint8_t>& data, bool sync);
//    void UploadCpuToGpu(const std::vector<uint8_t>& data, bool sync);

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
