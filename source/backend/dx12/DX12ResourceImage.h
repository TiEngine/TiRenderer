#pragma once

#include "backend/ResourceImage.h"
#include "DX12CommandRecorder.h"

namespace ti::backend {
class DX12Device;
class DX12ResourceImage : public ResourceImage {
public:
    explicit DX12ResourceImage(DX12Device& device);
    ~DX12ResourceImage() override;

    void Setup(Description description);
    void Shutdown();

    Microsoft::WRL::ComPtr<ID3D12Resource> Buffer();
    Microsoft::WRL::ComPtr<ID3D12Resource> Uploader();
    Microsoft::WRL::ComPtr<ID3D12Resource> Downloader();

protected:


private:
    DX12Device& internal;
    Microsoft::WRL::ComPtr<ID3D12Device> device;

    Description description{
        BasicFormat::R32G32B32A32_FLOAT,
        ImageType::Color, 0u, 0u };
    Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
    Microsoft::WRL::ComPtr<ID3D12Resource> uploader;
    Microsoft::WRL::ComPtr<ID3D12Resource> downloader;
    DX12CommandRecorder* transfer = nullptr;
};
}
